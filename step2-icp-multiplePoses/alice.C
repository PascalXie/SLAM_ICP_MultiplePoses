#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <sys/time.h>
#include "icp.h"
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;



float my_random(void);
Eigen::Matrix3d rotation_matrix(Eigen::Vector3d axis, float theta);
int uniform(Eigen::Vector3d & axis);
void test_icp3(double theta, Eigen::Vector3d axis);

Eigen::Matrix3d rotation_matrix_zAxis(float theta);
void my_random_shuffle(Eigen::MatrixXd &matrix);

void test_icp4(Eigen::MatrixXd B);

// global variables
Eigen::MatrixXd A_ = Eigen::MatrixXd::Random(N_pt,3);


unsigned GetTickCount()
{
		struct timeval tv;
		if(gettimeofday(&tv, NULL) != 0)
				return 0;

		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


ofstream file("ICP_Errors.txt"); 

int main(int argc, char *argv[]){

//	//
//	for(int i=0;i<1;i++)
//	{
//		double theta = rand()/double(RAND_MAX)*15./180.*M_PI;
//
//		Eigen::Vector3d axis = Eigen::Vector3d::Random();
//		int isUnifromationGood = uniform(axis);
//
//		test_icp3(theta, axis);
//	}
//
//	file.close();

	Eigen::MatrixXd A = A_;
	Eigen::MatrixXd B;
	Eigen::MatrixXd C;
	Eigen::Vector3d t;
	Eigen::Matrix3d R;
	Eigen::Matrix4d T;


	for(int i=0;i<6;i++)
	{

		//
		B = A;

		// translation vecgtor
		t << -0.2,0.8,0.3;

		// rotation angle, axis and matrix
		double theta = rand()/double(RAND_MAX)*15./180.*M_PI;

		Eigen::Vector3d axis = Eigen::Vector3d::Random();
		int isUnifromationGood = uniform(axis);

		Eigen::AngleAxisd rotationVector_1(theta,axis);
		R=rotationVector_1.toRotationMatrix();

		// translate the point
		for( int jj =0; jj< N_pt; jj++){
			B.block<1,3>(jj,0) = B.block<1,3>(jj,0) - t.transpose();
		}

		// rotate the point
		B = (R * B.transpose()).transpose();

		A = B;

		// noise
		//B += Eigen::MatrixXd::Random(N_pt,3) * noise_sigma;

		// shuffle
		my_random_shuffle(B);

		// debug
		cout<<"\n----"<<endl;
		cout<<"-----------------------"<<endl;
		cout<<"Pose ID "<<i<<endl;
		cout<<"----"<<endl;
		cout<<"ICP Ground Truth: "<<endl;
		cout<<"Translation vector t \n"<<t.transpose()<<endl;
		//cout<<"Rotation Matrix R \n"<<R<<endl;
		cout<<"Rotation angle: "<<theta/M_PI*180.<<" deg"<<endl;
		cout<<"Rotation axis: "<<axis.transpose()<<endl;

		test_icp4(B);

		// next
		A_ = A;
	}


	return 0;
}



///////////////////////////
//  help function

// 0-1 float variables
float my_random(void){
	float tmp = rand()%100;
	return tmp/1000;
}

void my_random_shuffle(Eigen::MatrixXd &matrix){
	int row = matrix.rows();
	vector<Eigen::Vector3d> temp;
	for(int jj=0; jj < row; jj++){
		temp.push_back(matrix.block<1,3>(jj,0));
	}
	random_shuffle(temp.begin(),temp.end());
	for(int jj=0; jj < row; jj++){
		matrix.block<1,3>(jj,0) = temp[jj].transpose();
		// cout << temp[jj].transpose() << endl;
		// cout << "row  " << row << endl;
	}
}


Eigen::Matrix3d rotation_matrix(Eigen::Vector3d axis, float theta){
	axis = axis / sqrt(axis.transpose()*axis);
	float a = cos(theta/2);
	Eigen::Vector3d temp = -axis*sin(theta/2);
	float b,c,d;
	b = temp(0);
	c = temp(1);
	d = temp(2);
	Eigen::Matrix3d R;
	R << a*a+b*b-c*c-d*d, 2*(b*c-a*d), 2*(b*d+a*c),
		2*(b*c+a*d), a*a+c*c-b*b-d*d, 2*(c*d-a*b),
		2*(b*d-a*c), 2*(c*d+a*b), a*a+d*d-b*b-c*c;

	return R;
}


Eigen::Matrix3d rotation_matrix_zAxis(float theta)
{
	double c = cos(theta);
	double s = sin(theta);

	Eigen::Matrix3d R;
	R<< c, s, 0.,
		-1.*s, c, 0.,
		0., 0., 1.;
	return R;
}

int uniform(Eigen::Vector3d & axis)
{
	// uniform the axis
	double norm2_squared = axis(0)*axis(0) + axis(1)*axis(1) + axis(2)*axis(2);
	double norm2 = sqrt(norm2_squared);

	axis(0) /= norm2;
	axis(1) /= norm2;
	axis(2) /= norm2;

	return 1;
}


void test_icp3(double theta, Eigen::Vector3d axis)
{
	//
	// points are rotated around an arbitrary axis
	//
	cout<<"\n---- "<<endl;
	cout<<"----------------"<<endl;
	cout<<"Function test_icp3 "<<endl;
	cout<<"points are rotated around an arbitraty axis"<<endl;
	cout<<"----------------"<<endl;
	cout<<"---- "<<endl;

	// variables that will be used in this function
	Eigen::MatrixXd A = Eigen::MatrixXd::Random(N_pt,3);
	Eigen::MatrixXd B;
	Eigen::MatrixXd C;
	Eigen::Vector3d t;
	Eigen::Matrix3d R;
	Eigen::Matrix4d T;
	Eigen::Vector3d t1;
	Eigen::Matrix3d R1;
	ICP_OUT icp_result;
	std::vector<float> dist;

	// debug
	cout<<"\n----"<<endl;
	cout<<"-----------------------"<<endl;
	cout<<"Function test_icp3"<<endl;
	//cout<<"Matrix A \n"<<A<<endl;
	// !debug

	B = A;
	//t = Eigen::Vector3d::Random()*translation;
	t << -0.2,0.8,0.3;

	// rotation
	Eigen::AngleAxisd rotationVector_1(theta,axis);
	R=rotationVector_1.toRotationMatrix();
	//cout<<"Rotation Matrix R \n"<<R<<endl;

	B = (R * B.transpose()).transpose();

	// translation 
	//cout<<"Translation vector t \n"<<t.transpose()<<endl;
	for( int jj =0; jj< N_pt; jj++){
		B.block<1,3>(jj,0) = B.block<1,3>(jj,0) - t.transpose();
	}

	// noise
	B += Eigen::MatrixXd::Random(N_pt,3) * noise_sigma;
	//cout<<"Measured Points that have been translated and biased: \n"<<B<<endl;

	// shuffle
	my_random_shuffle(B);

	// solve the ICP problem
	icp_result = icp(B, A, 20,  0.000001);
	//icp_result = icp(A, B, 20,  0.000001);

	// get Transformation matrix
	T = icp_result.trans;

	// get translation vector
	Eigen::Vector3d t_estimated = T.block<3,1>(0,3);

	// get Rotation Matrix
	Eigen::Matrix3d R_estimated_reverse = T.block<3,3>(0,0);
	Eigen::Matrix3d R_estimated = R_estimated_reverse.transpose();

	cout<<"\n----"<<endl;
	cout<<"-----------------------"<<endl;
	cout<<"ICP Ground Truth: "<<endl;
	cout<<"Translation vector t \n"<<t.transpose()<<endl;
	cout<<"Rotation Matrix R \n"<<R<<endl;
	cout<<"Rotation axis: "<<axis.transpose()<<endl;
	cout<<"Rotation angle: "<<theta/M_PI*180.<<" deg"<<endl;

	cout<<"\nICP result: "<<endl;
	cout<<"Transformation Matrix: \n"<<T<<endl;
	cout<<"Translation vector: \n"<<t_estimated.transpose()<<endl;
	cout<<"Rotation Matrix: \n"<<R_estimated<<endl;

	// eigen rotation matrix to angleAxis
	Eigen::AngleAxisd rotationVector;
	rotationVector.fromRotationMatrix(R_estimated);

	cout<<"Angle: "<<rotationVector.angle()/M_PI*180.<<endl;
	cout<<"Axis:  "<<rotationVector.axis().transpose()<<endl;

	// error 
	Eigen::Vector3d a_bar  = rotationVector.axis();
	Eigen::Vector3d a_true = axis;
	double e_axis = (a_bar-a_true).dot(a_bar-a_true);
	e_axis = sqrt(e_axis);
	cout<<"Error e_axis: "<<e_axis<<endl;

	double e_angle = abs(rotationVector.angle()-theta);
	e_angle = e_angle * M_PI * 180.;
	cout<<"Error e_angle: "<<e_angle<<endl;

	//
	// write errors into a file
	//
	file<<e_axis<<" "<<e_angle<<endl;


	return ;
}

void test_icp4(Eigen::MatrixXd B)
{
	Eigen::Matrix4d T;
	ICP_OUT icp_result;

	// solve the ICP problem
	icp_result = icp(B, A_, 20,  0.000001);
	//icp_result = icp(A_, B, 20,  0.000001);

	// get Transformation matrix
	T = icp_result.trans;

	// get translation vector
	Eigen::Vector3d t_estimated = T.block<3,1>(0,3);

	// get Rotation Matrix
	Eigen::Matrix3d R_estimated_reverse = T.block<3,3>(0,0);
	Eigen::Matrix3d R_estimated = R_estimated_reverse.transpose();

	cout<<"\nICP result: "<<endl;
	//cout<<"Transformation Matrix: \n"<<T<<endl;
	cout<<"Translation vector: \n"<<t_estimated.transpose()<<endl;
	//cout<<"Rotation Matrix: \n"<<R_estimated<<endl;

	// eigen rotation matrix to angleAxis
	Eigen::AngleAxisd rotationVector;
	rotationVector.fromRotationMatrix(R_estimated);

	cout<<"Angle: "<<rotationVector.angle()/M_PI*180.<<" deg"<<endl;
	cout<<"Axis:  "<<rotationVector.axis().transpose()<<endl;

}
