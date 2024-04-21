#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>


Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

void
Renderer::Render()
{
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

            image.setPixel(x, y, color);
            nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            }
        }
    }
    // END SOLN

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



Vector3f
Renderer::traceRay(const Ray &r,
    float tmin,
    int bounces,
    Hit &h) const
{
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.

    // TODO: IMPLEMENT 
    if(bounces==0&&_args.bounces!=0)
        return _scene.getBackgroundColor(r.getDirection());
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
                if(shadow){
                    color+=Vector3f::ZERO;
                }else color+=h.getMaterial()->shade(r,h,dirToLight,lightIntensity);
                //iterative
                
            }
        //std::cout<<color.x()<<" "<<color.y()<<" "<<color.z()<<std::endl;
        if(bounces>0){
                    Vector3f N= h.getNormal().normalized();
                    Vector3f L= -r.getDirection().normalized();
                    Vector3f R = 2*Vector3f::dot(N,L)*N-L;
                    Ray new_r(r.pointAtParameter(h.getT()),R);
                    Hit new_h;
                    color+=traceRay(new_r,tmin,bounces-1,new_h)*h.getMaterial()->getSpecularColor();
                }
        return color+ambientLight;
    } else {
        return _scene.getBackgroundColor(r.getDirection());
        std::cout<<"no hit"<<std::endl;
    };
}

