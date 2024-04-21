#include "Material.h"
#include "Object3D.h"
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{   
    //std::cout<<"lightIntensity"<<lightIntensity.x()<<lightIntensity.y()<<lightIntensity.z()<<std::endl;

    // TODO implement Diffuse and Specular phong terms

    Vector3f N= hit.getNormal().normalized();
    //std::cout<<"the normal is :"<<N.x()<<" "<<N.y()<<" "<<N.z()<<std::endl;
    Vector3f L= dirToLight;
    float champ=0;
    if(Vector3f::dot(L,N)<0) champ=0;
    else champ = Vector3f::dot(L,N);
    Vector3f diffuse = _diffuseColor*champ*lightIntensity;

    //then speclar
    Vector3f E = -ray.getDirection();
    Vector3f R = 2*Vector3f::dot(N,E)*N-E;
    auto temp = Vector3f::dot(L,R);
    Vector3f specular = pow(temp>0.0 ?temp:0.0,_shininess)*lightIntensity*_specularColor;

    return diffuse+specular;
}
