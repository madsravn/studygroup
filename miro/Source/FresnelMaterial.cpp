#include "FresnelMaterial.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"
#include "Matrix4x4.h"
#include <cmath>

struct mat2x2 {
	float n[2][2];
};

void multiply(mat2x2 a, mat2x2 b, mat2x2 &ret) {

	ret.n[0][0] = a.n[0][0]*b.n[0][0] + a.n[0][1]*b.n[1][0];
	ret.n[0][1] = a.n[0][0]*b.n[0][1] + a.n[0][1]*b.n[1][1];
	ret.n[1][0] = a.n[1][0]*b.n[0][0] + a.n[1][1]*b.n[1][0];
	ret.n[1][1] = a.n[1][0]*b.n[0][1] + a.n[1][1]*b.n[1][1];
}

FresnelMaterial::FresnelMaterial(const float index = 1.0f) {
	ior = index;
	reflectionMaterial = ReflectionMaterial();
	refractionMaterial = RefractionMaterial(index);
}

Vector3 FresnelMaterial::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene, int recDepth) const {

		Vector3 fresnelColor = Vector3(0.0f);
		Vector3 refractionColor = Vector3(0.0f);
		Vector3 reflectionColor = Vector3(0.0f);
		Vector3 erefractionColor = Vector3(0.0f);
		if (recDepth > 0) {


			// refraction

			refractionColor = Vector3(1.0f);
			erefractionColor = Vector3(1.0f);
			HitInfo refractionHit;
			HitInfo erefractionHit;

			// Taken from Wikipedia.org, taken from An Introduction to Ray Tracing by Andrew S. Glassner
			float my1 = scene.globalIoR;
			float my2 = ior;
			float costheta1 = dot(hit.N, -ray.d);
			float costheta2 = 1 - pow(my1/my2, 2) * (1 - pow(costheta1, 2));
			if (costheta2 >= 0) {
				costheta2 = sqrt(costheta2);
			} else {
				std::cout << "costheta2 sqrt error. Component is negative: " << costheta2 << std::endl;
				costheta2 = 0;
			}

			// extra-ordinary refraction

			float n_e = 1.5f; // extraordinary ior

			float n_2 = (my2 * n_e)/
				std::sqrt(pow(my2,2) * 
				pow(std::sin(std::acos(costheta1)),2) + 
				pow(n_e, 2) * pow(costheta1, 2));
			//std::cout << n_2 << std::endl;

			Vector3 veRefract;
			bool goingOut = false;
			if (costheta1 >= 0) {	// Going in
				veRefract = (my1/n_2) * ray.d + ((my1/n_2) * costheta1 - costheta2) * hit.N;
			} else {				// Going out
				veRefract = (n_2/my1) * ray.d - ((my1/n_2) * costheta1 - costheta2) * hit.N;
				goingOut = true;
			}
			veRefract.normalize();

			Vector3 vRefract;
			if (!goingOut) {	// Going in
				vRefract = (my1/my2) * ray.d + ((my1/my2) * costheta1 - costheta2) * hit.N;
			} else {				// Going out
				vRefract = (my2/my1) * ray.d - ((my1/my2) * costheta1 - costheta2) * hit.N;
			}
			vRefract.normalize();


			// reflection

			reflectionColor = Vector3(1.0f);
			HitInfo reflectionHit;
			Vector3 vReflect = ray.d + 2.0f * costheta1 * hit.N;

			float costheta_i = dot(hit.N, -ray.d);
			float costheta_t = dot(-hit.N, vRefract);
			float costheta_r = dot(hit.N, vReflect);

			// Fresnel coefficients
			float r_s = pow(
				(my1 * costheta_i - my2 * costheta_t)/
				(my1 * costheta_i + my2 * costheta_t),
				2);

			float t_s = pow(
				(2 * my1 * costheta_i)/
				(my1 * costheta_i + my2 * costheta_t),
				2);

			float r_p = pow(
				(my2 * costheta_i - my1 * costheta_t)/
				(my2 * costheta_i + my1 * costheta_t), 
				2);

			float t_p = pow(
				(2 * my1 * costheta_i)/
				(my1 * costheta_t + my2 * costheta_i),
				2);

			float reflectionCoefficient = (r_s + r_p) * (1.0f/2);
			float transmissionCoefficient = 1 - reflectionCoefficient; //((my2*costheta_t)/(my1*costheta_i)) * (pow(t_s, 2) + pow(t_p, 2)) * (1.0f/2);
			float T_s = ((my2*costheta_t)/(my1*costheta_i)) * pow(t_s, 2);
			float T_p = ((n_2*costheta_t)/(my1*costheta_i)) * pow(t_p, 2);	// Is this correct?

			/*	Matrix4x4 R_theta_i = Matrix4x4();
			R_theta_i.m11 = costheta_i;
			R_theta_i.m12 = sin(acos(costheta_i));
			R_theta_i.m21 = -sin(acos(costheta_i));
			R_theta_i.m22 = costheta_i;

			Matrix4x4 R_theta_t = Matrix4x4();
			R_theta_i.m11 = costheta_t;
			R_theta_i.m12 = sin(acos(costheta_t));
			R_theta_i.m21 = -sin(acos(costheta_t));
			R_theta_i.m22 = costheta_t;

			Matrix4x4 R_theta_r = Matrix4x4();
			R_theta_i.m11 = costheta_r;
			R_theta_i.m12 = sin(acos(costheta_r));
			R_theta_i.m21 = -sin(acos(costheta_r));
			R_theta_i.m22 = costheta_r;*/

			/*Matrix4x4 F_t = Matrix4x4();
			F_t.m11 = t_p;
			F_t.m12 = 0;
			F_t.m21 = 0;
			F_t.m22 = t_s;

			Matrix4x4 F_r = Matrix4x4();
			F_r.m11 = r_p;
			F_r.m12 = 0;
			F_r.m21 = 0;
			F_r.m22 = r_s;

			Matrix4x4 M_t = R_theta_i * F_t * R_theta_t;
			Matrix4x4 M_r = R_theta_i * F_r * R_theta_r;

			Matrix4x4 M_t_T = Matrix4x4(M_t);
			Matrix4x4 M_r_T = Matrix4x4(M_r);
			M_t_T.transpose();
			M_r_T.transpose();

			Matrix4x4 J_t = M_t * ray.J * M_t_T; 
			Matrix4x4 J_r = M_r * ray.J * M_r_T;
			*/
			Ray rayRefract = Ray(Vector3(hit.P), vRefract);
			Ray rayReflect = Ray(Vector3(hit.P), vReflect);

			Ray rayeRefract = Ray(Vector3(hit.P), veRefract);

			/*rayRefract.J = J_t;
			rayReflect.J = J_r;

			rayeRefract.J = J_t;*/

			// trace ordinary refraction
			if(scene.trace(refractionHit, rayRefract, 0.001f, 100.0f)) {
				refractionColor *= refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth - 1);

				/* Light Absorption */
				if(goingOut) {
					float distance = (hit.P - refractionHit.P).length();

					// affect colors, http://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law

					Vector3 absorption = Vector3(0.0056, 0.006, 0.0183)*10.0f;

					Vector3 lightTransmission = Vector3(
						pow(10, -absorption.x*distance),
						pow(10, -absorption.y*distance),
						pow(10, -absorption.z*distance));
					refractionColor *= (lightTransmission);
				}
			}
			else {
				//Image based
				refractionColor *= scene.getHDRColorFromVector(rayRefract.d);
			}


			// trace extraordinary refraction
			if(scene.trace(refractionHit, rayeRefract, 0.001f, 100.0f)) {
				erefractionColor *= refractionHit.material->shade(rayeRefract, refractionHit, scene, recDepth - 1);

				/* Light Absorption */
				if(goingOut) {
					float distance = (hit.P - erefractionHit.P).length();

					// affect colors, http://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law

					Vector3 absorption = Vector3(0.170, 0.175, 0.257)*10.0f;

					Vector3 lightTransmission = Vector3(
						pow(10, -absorption.x*distance),
						pow(10, -absorption.y*distance),
						pow(10, -absorption.z*distance));
					erefractionColor *= (lightTransmission);
				}
			}
			else {
				//Image based
				erefractionColor *= scene.getHDRColorFromVector(rayRefract.d);
			}

			// trace reflection
			if(scene.trace(reflectionHit, rayReflect, 0.001f, 100.0f)) {
				reflectionColor *= reflectionHit.material->shade(rayReflect, reflectionHit, scene, recDepth - 1);
			}
			else {
				//Image based
				reflectionColor *= scene.getHDRColorFromVector(rayReflect.d);
			}
			// transmissionCoefficient*(( refractionColor + refractionColor )/2

			//std::cout << "t_s: \t" << t_s << "\tt_p: \t" << t_p << std::endl;
			fresnelColor = reflectionCoefficient*reflectionColor + (T_s*refractionColor + T_p*erefractionColor);
		}

		return fresnelColor;
}
