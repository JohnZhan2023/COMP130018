#include "curve.h"
#include "vertexrecorder.h"
using namespace std;

const float c_pi = 3.14159265358979323846f;

namespace
{
// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vector3f& lhs, const Vector3f& rhs)
{
	const float eps = 1e-8f;
	return (lhs - rhs).absSquared() < eps;
}


}
// struct TMP{
// 	Vector3f tmp=Vector3f(0,0,1);
// };
// TMP tmp;

Curve evalBezier(const vector< Vector3f >& P, unsigned steps){
	// Check
	if (P.size() < 4 || P.size() % 3 != 1)
	{
		cerr << "evalBezier must be called with 3n+1 control points." << endl;
		exit(0);
	}
	int pieces = (P.size()-1) / 3;
	Curve R(pieces*(steps+1)-pieces+1);
	// construct the matrix
	Matrix4f M(1, -3, 3, -1,
				0, 3, -6, 3,
				0, 0, 3, -3,
				0, 0, 0, 1);
	// std::cout<<"the number of pieces:"<<pieces<<endl;
	// std::cout<<"the number of steps:"<<steps<<endl;
	cout<<"******************"<<endl;
	for(int j=0;j<pieces;j+=1){
		//cout<<"the j round "<<j<<" is done"<<endl;	
		Vector4f Px(P[0+j].x(), P[1+j].x(), P[2+j].x(), P[3+j].x());
		Vector4f Py(P[0+j].y(), P[1+j].y(), P[2+j].y(), P[3+j].y());
		Vector4f Pz(P[0+j].z(), P[1+j].z(), P[2+j].z(), P[3+j].z());
		float delta = 1.0f / steps;
		float t = 0;
		for(unsigned int i = 0; i < steps+1; i++){
			//cout<<"the i round "<<i<<" is done"<<endl;
			// cout<<"i+j*(steps) "<<i+j*(steps)<<endl;
			Vector4f pos(1, t, t*t, t*t*t);

			Vector4f weight = M * pos;
			R[i+j*(steps)].V = Vector3f(Vector4f::dot(Px,weight), Vector4f::dot(Py,weight), Vector4f::dot(Pz,weight));
			//cout<<"the "<<i<<" V is "<<R[i+j*(steps)].V.x()<<" "<<R[i+j*(steps)].V.y()<<" "<<R[i+j*(steps)].V.z()<<endl;
			Vector4f tangent(0, 1, 2*t, 3*t*t);
			Vector4f tangent_weight = M * tangent;
			R[i+j*(steps)].T = Vector3f(Vector4f::dot(Px,tangent_weight), Vector4f::dot(Py,tangent_weight),Vector4f::dot(Pz,tangent_weight)).normalized();
			//cout<<"the T is "<<R[i+j*(steps)].T.x()<<" "<<R[i+j*(steps)].T.y()<<" "<<R[i+j*(steps)].T.z()<<endl;

			if(i == 0){

			// If it is the first step, we should initialize the N
				Vector3f tmp=Vector3f(R[i+j*(steps)].T.x(),R[i+j*(steps)].T.y(),R[i+j*(steps)].T.z()+1.0f);
				
				R[i+j*(steps)].N = Vector3f::cross(tmp, R[i+j*(steps)].T).normalized();
				R[i+j*(steps)].B = Vector3f::cross(R[i+j*(steps)].T,R[i+j*(steps)].N).normalized();
				//cout<<"here?"<<endl;
			}
			else{
			// iteratively
				R[i+j*(steps)].N = Vector3f::cross(R[j*(steps)+i-1].B,R[i+j*(steps)].T).normalized();
				R[i+j*(steps)].B = Vector3f::cross(R[j*(steps)+i].T,R[i+j*(steps)].N).normalized();
				//cout<<"the N is "<<R[i+j*(steps)].N<<endl;
				
			}
			cout<<"the "<<i<<" N is "<<R[i+j*(steps)].N.x()<<" "<<R[i+j*(steps)].N.y()<<" "<<R[i+j*(steps)].N.z()<<endl;
			//cout<<"the B is "<<R[i+j*(steps)].B<<endl;
			t += delta;

		}
	}

	//cout<<"R's size is :"<<R.size()<<endl;
	//cout<<"the first T is "<<R[0].T.x()<<" "<<R[0].T.y()<<" "<<R[0].T.z()<<endl;	
	// TODO:
	// You should implement this function so that it returns a Curve
	// (e.g., a vector< CurvePoint >).  The variable "steps" tells you
	// the number of points to generate on each piece of the spline.
	// At least, that's how the sample solution is implemented and how
	// the SWP files are written.  But you are free to interpret this
	// variable however you want, so long as you can control the
	// "resolution" of the discretized spline curve with it.

	// Make sure that this function computes all the appropriate
	// Vector3fs for each CurvePoint: V,T,N,B.
	// [NBT] should be unit and orthogonal.

	// Also note that you may assume that all Bezier curves that you
	// receive have G1 continuity.  Otherwise, the TNB will not be
	// be defined at points where this does not hold.


	// cerr << "\t>>> evalBezier has been called with the following input:" << endl;

	// cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
	// for (int i = 0; i < (int)P.size(); ++i)
	// {
	// 	cerr << "\t>>> " << P[i] << endl;
	// }

	// cerr << "\t>>> Steps (type steps): " << steps << endl;
	// cerr << "\t>>> Returning empty curve." << endl;

	// Right now this will just return this empty curve.
	return R;
}

Curve evalBspline(const vector< Vector3f >& P, unsigned steps)
{	
	cout<<"the number of P:"<<P.size()<<endl;
	// Check
	if (P.size() < 4)
	{
		cerr << "evalBspline must be called with 4 or more control points." << endl;
		exit(0);
	}

	// TODO:
	// It is suggested that you implement this function by changing
	// basis from B-spline to Bezier.  That way, you can just call
	// your evalBezier function.
	Matrix4f M(1, -3, 3, -1,
				4, 0, -6, 3,
				1, 3, 3, -3,
				0, 0, 0, 1);
	M/=6;
	Matrix4f Mbezier(1, -3, 3, -1,
					0, 3, -6, 3,
					0, 0, 3, -3,
					0, 0, 0, 1);
	Matrix4f Mbezier_inv = Mbezier.inverse();
	Matrix4f GM = M * Mbezier_inv;

	Curve R;

	
	for(unsigned int i=3;i<P.size();i++){
		vector<Vector3f> Pnew;
		Vector4f Px(P[i-3].x(), P[i-2].x(), P[i-1].x(), P[i].x());
		Vector4f Py(P[i-3].y(), P[i-2].y(), P[i-1].y(), P[i].y());
		Vector4f Pz(P[i-3].z(), P[i-2].z(), P[i-1].z(), P[i].z());
		Pnew.push_back(Vector3f(Vector4f::dot(Px,GM.getCol(0)), Vector4f::dot(Py,GM.getCol(0)), Vector4f::dot(Pz,GM.getCol(0))));
		Pnew.push_back(Vector3f(Vector4f::dot(Px,GM.getCol(1)), Vector4f::dot(Py,GM.getCol(1)), Vector4f::dot(Pz,GM.getCol(1))));
		Pnew.push_back(Vector3f(Vector4f::dot(Px,GM.getCol(2)), Vector4f::dot(Py,GM.getCol(2)), Vector4f::dot(Pz,GM.getCol(2))));
		Pnew.push_back(Vector3f(Vector4f::dot(Px,GM.getCol(3)), Vector4f::dot(Py,GM.getCol(3)), Vector4f::dot(Pz,GM.getCol(3))));
		//tmp.tmp=Vector3f(R[R.size()-1].N.x()+1.1,R[R.size()-1].N.y(),R[R.size()-1].N.z());
		Curve R_=evalBezier(Pnew,steps);
		for(unsigned int j=0;j<R_.size();j++){
			R.push_back(R_[j]);
		}
		cout<<"The round "<< i<<" is done!!!!!"<<endl;
	}
	// cout<<"we complete the transformation"<<endl;
	// cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

	// cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
	// for (int i = 0; i < (int)P.size(); ++i)
	// {
	// 	cerr << "\t>>> " << P[i] << endl;
	// }

	// cerr << "\t>>> Steps (type steps): " << steps << endl;
	// cerr << "\t>>> Returning empty curve." << endl;

	// Return an empty curve right now.
	cout<<"R's size is :"<<R.size()<<endl;
	return R;
}

Curve evalCircle(float radius, unsigned steps)
{
	// This is a sample function on how to properly initialize a Curve
	// (which is a vector< CurvePoint >).

	// Preallocate a curve with steps+1 CurvePoints
	Curve R(steps + 1);

	// Fill it in counterclockwise
	for (unsigned i = 0; i <= steps; ++i)
	{
		// step from 0 to 2pi
		float t = 2.0f * c_pi * float(i) / steps;

		// Initialize position
		// We're pivoting counterclockwise around the y-axis
		R[i].V = radius * Vector3f(cos(t), sin(t), 0);

		// Tangent vector is first derivative
		R[i].T = Vector3f(-sin(t), cos(t), 0);

		// Normal vector is second derivative
		R[i].N = Vector3f(-cos(t), -sin(t), 0);

		// Finally, binormal is facing up.
		R[i].B = Vector3f(0, 0, 1);
	}

	return R;
}

void recordCurve(const Curve& curve, VertexRecorder* recorder)
{
	const Vector3f WHITE(1, 1, 1);
	for (int i = 0; i < (int)curve.size() - 1; ++i)
	{
		recorder->record_poscolor(curve[i].V, WHITE);
		recorder->record_poscolor(curve[i + 1].V, WHITE);
	}
}
void recordCurveFrames(const Curve& curve, VertexRecorder* recorder, float framesize)
{
	Matrix4f T;
	const Vector3f RED(1, 0, 0);
	const Vector3f GREEN(0, 1, 0);
	const Vector3f BLUE(0, 0, 1);
	
	const Vector4f ORGN(0, 0, 0, 1);
	const Vector4f AXISX(framesize, 0, 0, 1);
	const Vector4f AXISY(0, framesize, 0, 1);
	const Vector4f AXISZ(0, 0, framesize, 1);

	for (int i = 0; i < (int)curve.size(); ++i)
	{
		T.setCol(0, Vector4f(curve[i].N, 0));
		T.setCol(1, Vector4f(curve[i].B, 0));
		T.setCol(2, Vector4f(curve[i].T, 0));
		T.setCol(3, Vector4f(curve[i].V, 1));
 
		// Transform orthogonal frames into model space
		Vector4f MORGN  = T * ORGN;
		Vector4f MAXISX = T * AXISX;
		Vector4f MAXISY = T * AXISY;
		Vector4f MAXISZ = T * AXISZ;

		// Record in model space
		recorder->record_poscolor(MORGN.xyz(), RED);
		recorder->record_poscolor(MAXISX.xyz(), RED);

		recorder->record_poscolor(MORGN.xyz(), GREEN);
		recorder->record_poscolor(MAXISY.xyz(), GREEN);

		recorder->record_poscolor(MORGN.xyz(), BLUE);
		recorder->record_poscolor(MAXISZ.xyz(), BLUE);
	}
}

