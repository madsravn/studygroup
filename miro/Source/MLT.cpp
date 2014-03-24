#include "MLT.h"
#include "Constants.h"
#include <limits>
#include "Timer.h"

const int maxRecDepth  = Constants::MaxPathLength;
int samps = 0;
const int biasSamples = 1000;

MLT::MLT(Scene& scene, Image* image, Camera* camera, int pathSamples, ITracer* tracer) : scene(scene), img(image), cam(camera), samples(pathSamples), renderer(tracer) {
	
    MC.imageWidth = image->width();
    MC.imageHeight = image->height();
    for(int i = 0; i < 3*img->height()*img->width(); ++i) {
        picture.push_back(0.0f);
    }

    for(int i = 0; i < img->height(); ++i) {
        for(int j = 0; j < img->width(); ++j) {
            img->setPixel(j,i, Vector3(0.0f));
        }
    }
}

void MLT::run() {    

	/*
	function metropolisLightTransport()
		x <- initialPath()
		image <- { array of zeros }
		for i <- 1 to N
			y <- mutate(x)
			a <- acceptProb(x -> y
			if random < a
				then x <- y
			recordSample(image, x)
		return image	
	*/
   
    const int INTMAXHALF = std::numeric_limits<int>::max() / 2;

    double LargeStepProb = 0.3;

	double b = 1.0f;
	// Estimate normalization constant
	for (int i = 0; i < biasSamples; i++) {
		fprintf(stdout, "\rPSMLT Initializing: %5.2f", 100.0 * i / (biasSamples));
        fflush(stdout);
		MarkovChain normChain(img->width(), img->height());        
		b += renderer->calcPathContribution(MC).scalarContribution;
	}
    printf("\n");
	b /= double(biasSamples);	// average
	
    bool running = true;
    MarkovChain current(img->width(), img->height());
    MarkovChain proposal(img->width(), img->height());

    // Initialize current
    Ray tRay = cam->randomRay(img->width(), img->height(), current);
	current.contribution = renderer->calcPathContribution(current);
    int count = 0;
    int i = 0;

    // Paint over the geometry scene
    /*for(int j = 0; j < img->height(); ++j) {
        img->drawScanline(j);
        glFinish();
    }*/
    Timer t;
    t.start();
    while( running ) {
		samps++;
        double isLargeStepDone;
        if(rnd() <= LargeStepProb) {
            isLargeStepDone = 1.0;
            proposal = current.large_step(img->width(), img->height());
        } else {
            isLargeStepDone = 0.0;
            proposal = current.mutate(img->width(), img->height());
        }
		
		proposal.contribution = renderer->calcPathContribution(proposal);

		double a = acceptProb(current, proposal);

		// accumulate samples		

		if (proposal.contribution.scalarContribution > 0.0) 
			accumulatePathContribution(
					proposal.contribution, 
					(a + isLargeStepDone)/(proposal.contribution.scalarContribution/b + isLargeStepDone));
		if (current.contribution.scalarContribution > 0.0) 
			accumulatePathContribution(
					current.contribution, 
					(1.0 - a)/(current.contribution.scalarContribution/b + isLargeStepDone));

        if(rnd() <= a) {
            current = proposal;
        }
        
		i++;

        if(t.duration().count()/1000 > Constants::seconds) {
            running = false;
            std::cout << "Stopping after " << t.duration().count() << " ms." << std::endl;
			i = 0;
			for(int j = 0; j < img->height(); ++j) {
				img->drawScanline(j);
				glFinish();
			}

        }
		if(i % 1000000 == 0) {
			std::cout << "samps = " << samps << std::endl;

			for (int j = 0; j < img->height(); ++j) {
				img->drawScanline(j);
				glFinish();
			}
		}
        
		printf("Rendering Progress: %.3f%%\r", (t.duration().count() / 1000) / float(Constants::seconds)*100.0f);
        fflush(stdout);
    }	

	double s = double(img->width() * img->height()) / double(samps);
	
    for(int j = 0; j < img->height(); ++j) {
		for (int i = 0; i < img->width(); ++i) {

			int pixelpos = j*img->width() + i;

			Vector3 color = Vector3(picture[3 * pixelpos], picture[3 * pixelpos + 1], picture[3 * pixelpos + 2]) * s;

			img->setPixel(i, j, color);
		}
		img->drawScanline(j);
		glFinish();
    }

	char str[1024];
	sprintf(str, "PT_MLT_%ds", (t.duration().count() / 1000));

	scene.writeImg(str);
}

// Random mutation of a path
float MLT::mutate(float value) {
	//std::cout << "mutate" << std::endl;
	float s1 = 1.0f / 1024, s2 = 1.0f / 64;
	float dv = s2 * exp(-log(s2/s1)*rnd());
	if (rnd() < 0.5f) {
		value += dv;
		if(value > 1)
			value -= 1;		
	} else {
		value -= dv;
		if (value < 0) 
			value += 1;
	}
	return value;
}

// Builds an initial path
std::vector<HitInfo> initialPath() {
	//std::cout << "initialPath" << std::endl;
	return std::vector<HitInfo>();
}

Vector3 MLT::pathTraceFromPath(std::vector<HitInfo> path) const{
	// Recursive shading
	Vector3 shadeResult = Vector3(0.0f);
	
	if (path.size() >= 2) {
		shadeResult += path.at(1).material->shade(path, 1, scene);			
	}
		
	return shadeResult;
}

void MLT::accumulatePathContribution(const PathContribution pathContribution, const double scaling) {
	for (int i = 0; i < pathContribution.colors.size(); i++) {
		Contribution currentColor = pathContribution.colors.at(i);

		const int ix = int(currentColor.x);
		const int iy = int(currentColor.y);
		
		Vector3 color = currentColor.color * scaling;
		if (ix >= 0 && ix < img->width() && iy >= 0 && iy < img->height()) {	
            int pixelpos = iy*img->width() + ix;

			color += Vector3(picture[3 * pixelpos], picture[3 * pixelpos + 1], picture[3 * pixelpos + 2]);

			double s = (double)(img->width() * img->height()) / (double)samps;
            
            picture[3*pixelpos] = color.x;
            picture[3*pixelpos+1] = color.y;
            picture[3*pixelpos+2] = color.z;
			img->setPixel(ix, iy, color * s);
		}
	}
}

double MLT::acceptProb(MarkovChain& current, MarkovChain& proposal) const {
	// T(y > x) / T(x > y)	
	double a = 1.0;
	if (current.contribution.scalarContribution > 0.0){
		double cont_proposal = proposal.contribution.scalarContribution;
		double cont_current = current.contribution.scalarContribution;
		a = clamp(cont_proposal / cont_current, 0.0, 1.0);
	}
	return a;
}
