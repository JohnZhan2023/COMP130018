#include "Object3D.h"
#include "VecUtils.h"

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
        if(param.x()<=1&&param.x()>=0&&param.y()<=1&&param.y()>=0&&param.z()<=1&&param.z()>=0&&t<h.getT()){
            n=_normals[0]*param.x()+_normals[1]*param.y()+_normals[2]*param.z();
            h.set(t,this->material,n.normalized());
            return true;
        }else{
            return false;
        }
    
    }
    
}
Matrix4f operator + (Matrix4f m1, const Matrix4f& m2){
    Matrix4f m(m1.getCol(0)+m2.getCol(0),m1.getCol(1)+m2.getCol(1),m1.getCol(2)+m2.getCol(2),m1.getCol(3)+m2.getCol(3));
    return m;
}
Matrix4f operator - (Matrix4f m1, const Matrix4f& m2){
    Matrix4f m(m1.getCol(0)-m2.getCol(0),m1.getCol(1)-m2.getCol(1),m1.getCol(2)-m2.getCol(2),m1.getCol(3)-m2.getCol(3));
    return m;
}
void decomposeMatrix(Matrix4f M, Matrix4f& T, Matrix4f& S, Matrix4f& R) {
    //for a scaling and rotation transformation, we decompose it into a scaling matrix and a rotation matrix
    T = Matrix4f(1, 0, 0, M(0, 3),
                 0, 1, 0, M(1, 3),
                 0, 0, 1, M(2, 3),
                 0, 0, 0, 1);
    Matrix4f R_new(M(0,0),M(0,1),M(0,2),0,
                    M(1,0),M(1,1),M(1,2),0,
                    M(2,0),M(2,1),M(2,2),0,
                    0,0,0,1);
    //then we implement polar decomposition to find the R
    R=R_new;
    float dis=100;
    do{
        R_new = 0.5*(R +R.transposed().inverse());
        //R_new.print();
        Matrix4f delta = R_new-R;
        dis = delta.getCol(0).absSquared()+delta.getCol(1).absSquared()+delta.getCol(2).absSquared();
        R = R_new;
    }while(dis>0.001);
    S=R.inverse()*M;
    return ;
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
    Vector3f orig=VecUtils::transformPoint(M_inv,o);
    Vector3f dir=VecUtils::transformDirection(M_inv,d);
    Ray new_r(orig,dir);
    //std::cout<<"The new ray is :"<<new_r.getDirection().x()<<" "<<new_r.getDirection().y()<<" "<<new_r.getDirection().z()<<std::endl;
    //std::cout<<_object->getType()<<std::endl;
    bool result =_object->intersect(new_r,tmin,h);
    //we should conduct matrix decomposition
    //std::cout<<"The result is :"<<result<<std::endl;
    Matrix4f T,S,R;

    if(result){
        Vector3f n=h.getNormal().normalized();
        decomposeMatrix(M_inv,T,S,R);
        Vector3f n_global = VecUtils::transformDirection(M,n);
        //std::cout<<"the type:"<<this->_object->type<<std::endl;
        //R.print();
        if(dynamic_cast<Sphere*>(_object)){
            h.set(h.getT(),h.getMaterial(),VecUtils::transformDirection(T.inverse(),VecUtils::transformDirection(S,VecUtils::transformDirection(R.inverse(),n))));
            //std::cout<<"The normal is :"<<h.getNormal().x()<<" "<<h.getNormal().y()<<" "<<h.getNormal().z()<<std::endl;
        }else
        h.set(h.getT(),h.getMaterial(),n_global.normalized());
        //std::cout<<"The normal is :"<<h.getNormal().x()<<" "<<h.getNormal().y()<<" "<<h.getNormal().z()<<std::endl;
    }
    

    return result;
}

