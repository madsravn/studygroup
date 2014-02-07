#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"
//#include <mmintrin.h>
//#include <xmmintrin.h>
//#include <emmintrin.h>
#include <smmintrin.h>
Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{
    
}

Vector3
Triangle::getCenter() {
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle
    return  (v0+v1+v2)/3.0;
}

std::vector<Vector3> 
Triangle::getPoints() {
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle
    std::vector<Vector3> tmp;
    tmp.push_back(v0);
    tmp.push_back(v1);
    tmp.push_back(v2);
    /*std::cout << "RETURNING" << std::endl;
    std::cout << v0.x << ", " << v0.y << ", " << v0.z << std::endl;
    std::cout << v1.x << ", " << v1.y << ", " << v1.z << std::endl;
    std::cout << v2.x << ", " << v2.y << ", " << v2.z << std::endl;*/


    return tmp;
}


Triangle::~Triangle()
{

}

void
Triangle::preCalc() {

}

void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle


    // Normals from the three vertices
    TriangleMesh::TupleI3 tn3 = m_mesh->nIndices()[m_index];
    const Vector3 & na = m_mesh->normals()[tn3.x];
    const Vector3 & nb = m_mesh->normals()[tn3.y];
    const Vector3 & nc = m_mesh->normals()[tn3.z];


    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();

    /*
    glBegin(GL_LINES);
        glVertex3f(v0.x,v0.y,v0.z);
        glVertex3f(v0.x+na.x, v0.y+na.y,v0.z+na.z);
    glEnd();
    */
}

float determinant(const Vector3& a, const Vector3& b, const Vector3& c) {
	return dot(cross(a,b),c);
}

bool
Triangle::intersect(HitInfo& result, const Ray& ray,float tMin, float tMax)
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & a = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & b = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & c = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    
    // Cramers rule triangle intersection
    // The three vectors used in cramers rule along with ray.d
    const Vector3 aminuso(a-ray.o);
    const Vector3 aminusb(a-b);
    const Vector3 aminusc(a-c);
    
    // Checking for division with 0 
    const float determinantAll = determinant(aminusb,aminusc,ray.d);
    if(determinantAll == 0) { return false; }
    const float det = 1.0f/determinantAll;
    
    // Calculatin alpha, beta, gamma and t with cramers rule
    const float beta = determinant(aminuso,aminusc,ray.d)*det;
    const float gamma = determinant(aminusb,aminuso,ray.d)*det;
    const float t = determinant(aminusb,aminusc,aminuso)*det;
    const float alpha = 1 - beta - gamma;
    
	/*
    // Cramers rule using intrinsics
    const __m128 ia = _mm_setr_ps(a.x, a.y,a.z,0.0f);
    const __m128 ib = _mm_setr_ps(b.x,b.y,b.z,0.0f);
    const __m128 ic = _mm_setr_ps(c.x,c.y,c.z,0.0f);
    const __m128 id = _mm_setr_ps(ray.d.x,ray.d.y,ray.d.z,0.0f);
    const __m128 io = _mm_setr_ps(ray.o.x,ray.o.y,ray.o.z,0.0f);
    // Technique being used: http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
    const __m128 edge1 = _mm_sub_ps(ib,ia);
    const __m128 edge2 = _mm_sub_ps(ic,ia);
    // Credit for cross product: http://fastcpp.blogspot.dk/2011/04/vector-cross-product-using-sse-code.html
    const __m128 pvec = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(id, id, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(3, 1, 0, 2))), _mm_mul_ps(_mm_shuffle_ps(id, id, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(3, 0, 2, 1))));
    // Dot product: http://www.lst.inf.ethz.ch/teaching/lectures/hs11/2800/slides/2011_10_05_simd.pdf
    const __m128 idet = _mm_dp_ps(edge1,pvec,117);
    float a_idet[3];
    _mm_store_ps(a_idet,idet);
    if(a_idet[0] == 0.0f) {
        return false;
    }
    const __m128 one = _mm_setr_ps(1.0,1.0,1.0,0.0f);
    const __m128 inv_det = _mm_div_ps(one,idet);
    const __m128 tvec = _mm_sub_ps(io,ia);
    const __m128 iu = _mm_mul_ps(_mm_dp_ps(tvec,pvec,117),inv_det);
    const __m128 qvec = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(tvec, tvec, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(3, 1, 0, 2))), _mm_mul_ps(_mm_shuffle_ps(tvec, tvec, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(3, 0, 2, 1))));
    const __m128 iv = _mm_mul_ps(_mm_dp_ps(id,qvec,117),inv_det);
    const __m128 it = _mm_mul_ps(_mm_dp_ps(edge2,qvec,117),inv_det);
    float a_v[3];
    float a_u[3];
    float a_t[3];
    _mm_store_ps(a_v, iv);
    _mm_store_ps(a_u, iu);
    _mm_store_ps(a_t, it);
    // a_v[0] = gamma
    // a_u[0] = beta
    // a_t[0] = t
    const float gamma = a_v[0];
    const float beta = a_u[0];
    //const float t = a_t[0];
    const float alpha = 1-gamma-beta;
	*/

    /*
    // Signed volumes triangle intersection
    const Vector3 q(ray.o+ray.d);
    // the dot product is commutative
    const float Vc = (1.0f/6.0f)*determinant(b-ray.o,a-ray.o,q-ray.o);
    const float Vb = (1.0f/6.0f)*determinant(a-ray.o,c-ray.o,q-ray.o);
    const float Va = (1.0f/6.0f)*determinant(c-ray.o,b-ray.o,q-ray.o);
    const float alpha = Va/(Va+Vb+Vc);
    const float beta = Vb/(Va+Vb+Vc);
    const float gamma = Vc/(Va+Vb+Vc);
    */
    
	
	
	// To find p and t with signed volumes
    //const Vector3 p = alpha*a+beta*b+gamma*c;
    //const float t = dot(p-ray.o,ray.d);
    //const float t = (p-ray.o).length();
        
    // Checking constraints 
    if(1 > beta && beta > 0 && 1 > gamma && gamma > 0 && t > tMin && tMax > t && alpha > 0 && 1 > alpha ) {
        
        // The t_closest > t constraint is checked in BVH.cpp
        
        // Normals from the three vertices
        TriangleMesh::TupleI3 tn3 = m_mesh->nIndices()[m_index];
        const Vector3 & na = m_mesh->normals()[tn3.x];
        const Vector3 & nb = m_mesh->normals()[tn3.y];
        const Vector3 & nc = m_mesh->normals()[tn3.z];
    
        // Setting t and P
        result.t = t; 
        result.P = ray.o + result.t*ray.d;
        
        // Interpolating normal and normalizing it
        result.N = alpha*na + beta*nb + gamma*nc;
        result.N.normalize();

        /*Vector3 normal = cross(a-b,a-c);
        if(dot(normal, alpha*na + beta*b + gamma*nc) < 0) {
            normal = -normal;
        }
        
        result.N = normal.normalize();*/

        // Shading material
        result.material = this->m_material;
        
        return true;
    }

    return false;
}
