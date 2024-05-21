#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"
#include <random>

#include <limits>


Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

void
Renderer::Render()
{   
    if(_args.filter&&_args.jitter){
    //std::cout<<"filter and jitter"<<std::endl;
    int w = _args.width;
    int h = _args.height;
    int k=3;
    int w_=w*k;
    int h_=h*k;

    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    Camera* cam = _scene.getCamera();
    Vector3f** matrix = new Vector3f*[h_];
    for (int i = 0; i < h_; ++i) {
        matrix[i] = new Vector3f[w_];
    }
    Hit **matrix_h= new Hit*[h_];
    for(int i=0;i<h_;i++){
        matrix_h[i]=new Hit[w_];
    }
    
    // 使用随机设备作为种子
    std::random_device rd;
    // 使用 Mersenne Twister 引擎
    std::mt19937 gen(rd());
    // 均匀分布在 0 到 1 之间的实数
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    for (int y = 0; y < h_; ++y) {
        // double random_number1 = dis(gen);
        float ndcy = 2 * ((y) / (h_ - 1.0f)) - 1.0f;

        for (int x = 0; x < w_; ++x) {
            //double random_number2 = dis(gen);
            
            float ndcx = 2 * ((x) / (w_ - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Vector3f color=Vector3f::ZERO;
            Hit H;
            // printf("ndcx=%d,ndcy=%d\n",ndcx,ndcy);
            for(int i=0;i<16;i++){
                double random_number = dis(gen);
                // std::cout<<random_number<<std::endl;
                // std::cout<<ndcx+random_number/w_<<" "<<ndcy+random_number/h_<<std::endl;
                Ray r = cam->generateRay(Vector2f(ndcx+random_number/w_, ndcy+random_number/h_));
                //std::cout<<x<<" "<<y<<" "<<std::endl;
                Hit h;
                //std::cout<<r.getDirection().x()<<" "<<r.getDirection().y()<<" "<<r.getDirection().z()<<std::endl;
                Vector3f new_color = traceRay(r, cam->getTMin(), _args.bounces, h);
                //std::cout<<new_color.x()<<" "<<new_color.y()<<" "<<new_color.z()<<std::endl;
                H=h;
                color = color + new_color;
                
            }
            //std::cout<<color.x()<<" "<<color.y()<<" "<<color.z()<<std::endl;
            color=color/16.0;
            
            matrix[y][x]=color;
            matrix_h[y][x]=H;
            // color=color/16;
            // image.setPixel(x, y, color);
            // nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            // float range = (_args.depth_max - _args.depth_min);
            // if (range) {
            //     dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            // }
        }
    }
    
    for(int y =0;y<h;y++){
        for(int x=0;x<w;x++){
            Vector3f color=matrix[y*k][x*k]+matrix[y*k][x*k+1]*2+matrix[y*k][x*k+2]+matrix[y*k+1][x*k]*2+matrix[y*k+1][x*k+1]*4+matrix[y*k+1][x*k+2]*2+matrix[y*k+2][x*k]+matrix[y*k+2][x*k+1]*2+matrix[y*k+2][x*k+2];
            color/=16.0;
            //std::cout<<color.x()<<" "<<color.y()<<" "<<color.z()<<std::endl;
            image.setPixel(x,y,color);
            Vector3f normal=matrix_h[y*k][x*k].getNormal()+matrix_h[y*k][x*k+1].getNormal()*2+matrix_h[y*k][x*k+2].getNormal()+matrix_h[y*k+1][x*k].getNormal()*2+matrix_h[y*k+1][x*k+1].getNormal()*4+matrix_h[y*k+1][x*k+2].getNormal()*2+matrix_h[y*k+2][x*k].getNormal()+matrix_h[y*k+2][x*k+1].getNormal()*2+matrix_h[y*k+2][x*k+2].getNormal();
            float t = matrix_h[y*k][x*k].getT()+matrix_h[y*k][x*k+1].getT()*2+matrix_h[y*k][x*k+2].getT()+matrix_h[y*k+1][x*k].getT()*2+matrix_h[y*k+1][x*k+1].getT()*4+matrix_h[y*k+1][x*k+2].getT()*2+matrix_h[y*k+2][x*k].getT()+matrix_h[y*k+2][x*k+1].getT()*2+matrix_h[y*k+2][x*k+2].getT();
            normal/=16;
            t/=16;
            nimage.setPixel(x, y, (normal + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.setPixel(x, y, Vector3f((t - _args.depth_min) / range));
            }
        }
    }
    // save the files 
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
    }else{
    int w = _args.width;
    int h = _args.height;

    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    Camera* cam = _scene.getCamera();
    for (int y = 0; y < h; ++y) {
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Ray r = cam->generateRay(Vector2f(ndcx, ndcy));
            //std::cout<<x<<" "<<y<<" "<<std::endl;
            Hit h;
            //std::cout<<r.getDirection().x()<<" "<<r.getDirection().y()<<" "<<r.getDirection().z()<<std::endl;

            Vector3f color = traceRay(r, cam->getTMin(), _args.bounces, h);
            //std::cout<<color.x()<<" "<<color.y()<<" "<<color.z()<<std::endl;

            image.setPixel(x, y, color);
            nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            }
        }
    }
    // save the files 
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
    }
    // END SOLN


    }




Vector3f
Renderer::traceRay(const Ray &r,
    float tmin,
    int bounces,
    Hit &h) const
{
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.

    // TODO: IMPLEMENT 
    // if(_args.bounces==0){
    //     return _scene.getBackgroundColor(r.getDirection());
    // }
    //std::cout<<"traceRay"<<std::endl;
    if(bounces==0&&_args.bounces!=0)
        //std::cout<<h.getMaterial()->getDiffuseColor().x()<<" "<<h.getMaterial()->getDiffuseColor().y()<<" "<<h.getMaterial()->getDiffuseColor().z()<<std::endl;
        // return  _scene.getAmbientLight()*h.getMaterial()->getDiffuseColor();
        return Vector3f::ZERO;

    if (_scene.getGroup()->intersect(r, tmin, h)) {
        Vector3f ambientLight= _scene.getAmbientLight();
        Vector3f color= Vector3f::ZERO;
        for(int i=0;i<_scene.getNumLights();i++){
                Light* light = _scene.getLight(i);
                Vector3f dirToLight;
                Vector3f lightIntensity;
                float tmax = std::numeric_limits<float>::infinity();
                light->getIllumination(r.pointAtParameter(h.getT()),dirToLight,lightIntensity,tmax);
                bool shadow=false;
                Ray shadowRay(r.pointAtParameter(h.getT()),dirToLight);
                Hit h_;
                if(_scene.getGroup()->intersect(shadowRay,tmin,h_)){
                    shadow=true;
                }
                if(shadow&&_args.shadows){
                    color+=Vector3f::ZERO;
                }else color+=h.getMaterial()->shade(r,h,dirToLight,lightIntensity);
                //iterative
                
            }
        //std::cout<<color.x()<<" "<<color.y()<<" "<<color.z()<<std::endl;
        //std::cout<<bounces<<std::endl;
        if(bounces>0){
                    Vector3f N= h.getNormal().normalized();
                    Vector3f L= -r.getDirection().normalized();
                    Vector3f R = 2*Vector3f::dot(N,L)*N-L;
                    Ray new_r(r.pointAtParameter(h.getT()),R);
                    Hit new_h;
                    color+=traceRay(new_r,tmin,bounces-1,new_h)*h.getMaterial()->getSpecularColor();
                }
        if(_args.bounces==bounces)
            return color+ambientLight*h.getMaterial()->getDiffuseColor();
        else return color;
    } else {
        return _scene.getBackgroundColor(r.getDirection());
    };
}

