#include "surf.h"
#include "vertexrecorder.h"
using namespace std;

const float c_pi = 3.14159265358979323846f;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

// DEBUG HELPER
Surface quad() { 
	Surface ret;
	ret.VV.push_back(Vector3f(-1, -1, 0));
	ret.VV.push_back(Vector3f(+1, -1, 0));
	ret.VV.push_back(Vector3f(+1, +1, 0));
	ret.VV.push_back(Vector3f(-1, +1, 0));

	ret.VN.push_back(Vector3f(0, 0, 1));
	ret.VN.push_back(Vector3f(0, 0, 1));
	ret.VN.push_back(Vector3f(0, 0, 1));
	ret.VN.push_back(Vector3f(0, 0, 1));

	ret.VF.push_back(Tup3u(0, 1, 2));
	ret.VF.push_back(Tup3u(0, 2, 3));
	return ret;
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
	//surface = quad();
    //
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }
    float delta = 2.0f * c_pi / steps;
    int num = profile.size()*(steps);
    int total = num;
    for(unsigned int i=0;i<=steps ;i++){
        //cout<<"the i round "<<i<<" is done"<<endl;
        float theta = i * delta;
        Matrix4f M(cos(theta), 0, sin(theta), 0,
                    0, 1, 0, 0,
                    -sin(theta), 0, cos(theta), 0,
                    0, 0, 0, 1);
        Matrix4f M_inverse_transpose = M.inverse();
        M_inverse_transpose.transpose();
        
        for(int j=0;j<(int)profile.size();j++){
            Vector3f pos = profile[j].V;
            Vector3f normal = profile[j].N;
            Vector4f pos4(pos[0], pos[1], pos[2], 1);
            Vector4f normal4(normal[0], normal[1], normal[2], 0);

            Vector4f pos_new = M * pos4;

            Vector4f normal_new = (M_inverse_transpose * normal4);
            surface.VV.push_back(Vector3f(pos_new[0], pos_new[1], pos_new[2]));
            surface.VN.push_back(Vector3f(-normal_new[0], -normal_new[1], -normal_new[2]).normalized());
        }
        
        if(i==steps){
            break;
        }
        for(int j=0;j<(int)profile.size()-1;j++){
            Tup3u face1;
            Tup3u face2;
            face1[0] = (i * profile.size() + j)%total;
            face1[1] = (i * profile.size() + j + 1)%total;
            face1[2] = ((i + 1) * profile.size() + j )%total;
            face2[0] = (i * profile.size() + j + 1)%total;
            face2[1] = ((i + 1) * profile.size() + j + 1)%total;
            face2[2] = ((i + 1) * profile.size() + j)%total;
            surface.VF.push_back(face1);
            surface.VF.push_back(face2);
            // cout<<"the face1 is "<<face1[0]<<" "<<face1[1]<<" "<<face1[2]<<endl;
            //cout<<"the face2 is "<<face2[0]<<" "<<face2[1]<<" "<<face2[2]<<endl;
        
        }


        //cout<<"the size of VV is "<<surface.VV.size()<<endl;    

    }

    // TODO: Here you should build the surface.  See surf.h for details.

    //cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;
	//surface = quad();

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    int num=sweep.size();
    int num_profile = profile.size();
    int total = num * num_profile;
    for(int i = 0;i<num;i++){
        Matrix4f M(sweep[i].N[0], sweep[i].B[0], sweep[i].T[0], sweep[i].V[0],
                sweep[i].N[1], sweep[i].B[1], sweep[i].T[1], sweep[i].V[1],
                sweep[i].N[2], sweep[i].B[2], sweep[i].T[2], sweep[i].V[2],
                0, 0, 0, 1);
        Matrix4f M_inverse_transpose = M.inverse();
        M_inverse_transpose.transpose();
        for(int j = 0;j<num_profile;j++){
            Vector3f pos = profile[j].V;
            Vector3f normal = profile[j].N;
            Vector4f pos4(pos[0], pos[1], pos[2], 1);
            Vector4f normal4(normal[0], normal[1], normal[2], 0);

            Vector4f pos_new = M * pos4;

            Vector4f normal_new = (M_inverse_transpose * normal4);
            surface.VV.push_back(Vector3f(pos_new[0], pos_new[1], pos_new[2]));
            surface.VN.push_back(Vector3f(-normal_new[0], -normal_new[1], -normal_new[2]).normalized());
        }
        for(int j=0;j<num_profile;j++){
            Tup3u face1;
            Tup3u face2;
            face1[0] = (i * profile.size() + j)%total;
            face1[1] = (i * profile.size() + j + 1)%total;
            face1[2] = ((i + 1) * profile.size() + j )%total;
            face2[0] = (i * profile.size() + j + 1)%total;
            face2[1] = ((i + 1) * profile.size() + j + 1)%total;
            face2[2] = ((i + 1) * profile.size() + j)%total;

            surface.VF.push_back(face1);
            surface.VF.push_back(face2);
            // cout<<"the face1 is "<<face1[0]<<" "<<face1[1]<<" "<<face1[2]<<endl;
            // cout<<"the face2 is "<<face2[0]<<" "<<face2[1]<<" "<<face2[2]<<endl;
        
        }
    }

    
    //cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

void recordSurface(const Surface &surface, VertexRecorder* recorder) {
	const Vector3f WIRECOLOR(0.4f, 0.4f, 0.4f);
    for (int i=0; i<(int)surface.VF.size(); i++)
    {
		recorder->record(surface.VV[surface.VF[i][0]], surface.VN[surface.VF[i][0]], WIRECOLOR);
		recorder->record(surface.VV[surface.VF[i][1]], surface.VN[surface.VF[i][1]], WIRECOLOR);
		recorder->record(surface.VV[surface.VF[i][2]], surface.VN[surface.VF[i][2]], WIRECOLOR);
    }
}

void recordNormals(const Surface &surface, VertexRecorder* recorder, float len)
{
	const Vector3f NORMALCOLOR(0, 1, 1);
    for (int i=0; i<(int)surface.VV.size(); i++)
    {
		recorder->record_poscolor(surface.VV[i], NORMALCOLOR);
		recorder->record_poscolor(surface.VV[i] + surface.VN[i] * len, NORMALCOLOR);
    }
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (int i=0; i<(int)surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (int i=0; i<(int)surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (int i=0; i<(int)surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
