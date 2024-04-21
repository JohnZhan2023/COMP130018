#include "Object3D.h"

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER

    // We provide sphere intersection code for you.
    // You should model other intersection implementations after this one.

    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f*a);
    float tminus = (-b - d) / (2.0f*a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back 
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    // END STARTER
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const
{   
    // BEGIN STARTER
    // we implemented this for you
    bool hit = false;
    for (Object3D* o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
        }
    }
    return hit;
    // END STARTER
}


Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    // TODO implement Plane constructor
    n=normal;
    dist=d;
    
}
bool Plane::intersect(const Ray &r, float tmin, Hit &h) const
{   
    //std::cout<<"Plane intersect"<<std::endl;
    float t ;
    Vector3f o=r.getOrigin();
    Vector3f d=r.getDirection();
    //Then we sample a random point from the plane
    Vector3f p_=Vector3f(dist*n);
    // TODO implement
    if(Vector3f::dot(d,n)==0)
    {
        return false;
    }else{
        t=Vector3f::dot(p_-o,n)/Vector3f::dot(d,n);
        if(t>=tmin&&t<h.getT()){
            h.set(t,this->material,n);
            return true;
        }else{
            return false;
        }
    }
    
}
bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const 
{
    // TODO implement
    //std::cout<<"Triangle intersect"<<std::endl;
    Vector3f ab=Vector3f(_v[0]-_v[1]);
    Vector3f ac=Vector3f(_v[1]-_v[2]);
    Vector3f n = Vector3f::cross(ab,ac).normalized();
    Vector3f d=r.getDirection();
    Vector3f o=r.getOrigin();
    if(Vector3f::dot(n,d)==0){
        return false;
    }else{
        float t=Vector3f::dot(n,_v[0]-o)/Vector3f::dot(n,d);
        if(t<tmin){
            return false;
        }
        Vector3f p = o+t*d;
        Vector3f param=Vector3f::ZERO;
        Matrix3f A=Matrix3f(_v[0],_v[1],_v[2],1);
        Matrix3f A_inv = A.inverse();
        param = A_inv*p;
        if(param.x()<1&&param.x()>0&&param.y()<1&&param.y()>0&&param.z()<1&&param.z()>0&&t<h.getT()){
            h.set(t,this->material,n);
            return true;
        }else{
            return false;
        }
    
    }
    
}


Transform::Transform(const Matrix4f &m,
    Object3D *obj) : _object(obj) {
    // TODO implement Transform constructor
    M=m;

}
bool Transform::intersect(const Ray &r, float tmin, Hit &h) const
{
    // TODO implement
    //std::cout<<"The old ray is :"<<r.getDirection().x()<<" "<<r.getDirection().y()<<" "<<r.getDirection().z()<<std::endl;    
    Matrix4f M_inv= M.inverse();
    Vector3f o=r.getOrigin();
    Vector3f d=r.getDirection();
    Vector4f o_=Vector4f(o.x(),o.y(),o.z(),1);
    Vector4f d_=Vector4f(d.x(),d.y(),d.z(),0);
    Vector4f o_local = M_inv*o_;
    Vector4f d_local = M_inv*d_;
    Vector3f orig=Vector3f(o_local.x(),o_local.y(),o_local.z());
    Vector3f dir=Vector3f(d_local.x(),d_local.y(),d_local.z());
    Ray new_r(orig,dir);
    //std::cout<<"The new ray is :"<<new_r.getDirection().x()<<" "<<new_r.getDirection().y()<<" "<<new_r.getDirection().z()<<std::endl;
    //std::cout<<_object->getType()<<std::endl;
    bool result =_object->intersect(new_r,tmin,h);

    //std::cout<<"The result is :"<<result<<std::endl;
    if(result){
        Vector3f n=h.getNormal().normalized();
        Vector4f n_=Vector4f(n.x(),n.y(),n.z(),1);
        Vector4f n_global = M*n_;
        h.set(h.getT(),h.getMaterial(),Vector3f(n_global.x(),n_global.y(),n_global.z()));
        //std::cout<<"The normal is :"<<h.getNormal().x()<<" "<<h.getNormal().y()<<" "<<h.getNormal().z()<<std::endl;
    }
    

    return result;
}