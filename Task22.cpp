//---------------------------------------------// 
#include "stdafx.h"                             
//---------------------------------------------//
#include "Task22.h"                
//---------------------------------------------// 
#include "Properties.h"      
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"     
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> 
using namespace std;
//---------------------------------------------//
                                           
                                          
IMPLEMENT_GEOX_CLASS( Task22, 0 )    
{                                               
	BEGIN_CLASS_INIT( Task22 );       

	ADD_INT32_PROP(NumSamples, 0)

	//input values slope and number of points
	ADD_SEPARATOR("Negative slope")
	ADD_FLOAT32_PROP(slope, 0)
	ADD_FLOAT32_PROP(Yorigo,0)

	ADD_NOARGS_METHOD(Task22::NegativeSlope)   
	ADD_NOARGS_METHOD(Task22::Paraline)
}                                               

QWidget* Task22::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task22::Task22()          
{          
	viewer = NULL;

	//File Name
	fileName="cars.csv";

	//Data from the file
	

	//scatterplot axis
	scatterOrigin = makeVector2f(-1.5,0.0);
	endAxisX = makeVector2f(-0.5,0.0);
	endAxisY = makeVector2f(-1.5,1.0);
	endAxisYDown = makeVector2f(-1.5,-1.0);

	//parallel cord axis
	startX1 = makeVector2f(0.5,-1.0);
	endX1 = makeVector2f(0.5,1.0);
	startX2 = makeVector2f(1.5,-1.0);
	endX2 = makeVector2f(1.5,1.0);

	NumSamples = 10;

	//slope values
	slope=-1;
	Yorigo = 1;

	//testing load file
	LoadFile();
}                                               
                                              
Task22::~Task22()        
{             
}   

void Task22::LoadFile()
{
	ifstream file;
	file.open(fileName);
	vector<string> dataString;
	vector<string> variableName;
	vector<vector<float>> variableData;

	if(file.is_open()){
		//if successfully open read the file
		while(!file.eof()){
			string line;
			file>>line;
			dataString.push_back(line);
		}
		file.close();
		dataString.pop_back(); //trick to remove the last element that is an empty string
		
		//Organizing the elements read into their respective array

		//separating the name of the variables
		string firstLine= dataString[0];

		//check if there is comma (improve checking afterwards)
		int commaPosition=firstLine.find_first_of(',',0);
		while(commaPosition!= std::string::npos){
			commaPosition=firstLine.find_first_of(',',0);
			variableName.push_back(firstLine.substr(0, commaPosition));
			//Removing the recognized variable from the string
			firstLine.erase(0,commaPosition+1);
		}
		
		//separating the values of the variables
		
		for(int i=1;i<dataString.size() ;i++){
			//starts from the second element because the first line is the name of the variables
			vector<float> valuesReadLine;
			string analizedLine= dataString[i];
			commaPosition=analizedLine.find_first_of(',',0);
			while(commaPosition!= std::string::npos){
				commaPosition=analizedLine.find_first_of(',',0);
				float valueVariable;
				valueVariable=stof(analizedLine.substr(0, commaPosition));

				//Tranforming the read string value into float and putting into the array
				valuesReadLine.push_back(valueVariable);
				//Removing the recognized variable from the string
				analizedLine.erase(0,commaPosition+1);
			}
			variableData.push_back(valuesReadLine);
		}
	}

}
                                               
void Task22::CreateAxis()        
{                        
	Vector4f color = makeVector4f(1,1,1,1);
	//scatter plot axes
	viewer->addLine(scatterOrigin, endAxisX, color, 2);
	viewer->addLine(scatterOrigin, endAxisY, color, 2);
	viewer->addLine(scatterOrigin, endAxisYDown, color, 2);

	//parallel coordinates plot "axes"
	viewer->addLine(startX1, endX1, color, 2);
	viewer->addLine(startX2, endX2, color, 2);
}    

void Task22::CreateParaline(float xvalue, float yvalue){
	//make parallel line between points
	// X1 = XparStart, pointXvalue
	// Y1 = YparStart, pointYvalue

	Vector2f X1 = makeVector2f(startX1[0],(xvalue-scatterOrigin[0]));
	Vector2f Y1 = makeVector2f(startX2[0],(yvalue-scatterOrigin[1]));

	viewer->addLine( X1, Y1);
}

void Task22::NegativeSlope()
{

	viewer->clear();
	CreateAxis();

	float pointX= scatterOrigin[0];
	float pointY= Yorigo;

	Vector4f color=makeVector4f(1,0,0,1);
	int size= 5;
	
	float pace=(endAxisX[0]-scatterOrigin[0])/NumSamples;

	for(int i=0; i<=NumSamples; i++){ 
		Vector2f scatterPoint= makeVector2f(pointX,pointY);
		viewer->addPoint(scatterPoint);

		CreateParaline(pointX, pointY);

		pointX+= pace;
		pointY= slope*(pointX-scatterOrigin[0])+Yorigo;
	}

	// display changes
    viewer->refresh();
}

void Task22::Paraline(){
	viewer->clear();
	Vector4f color = makeVector4f(1,1,1,1);

	vector<float> arr;
	arr.push_back(1);
	arr.push_back(2);
	arr.push_back(3);
	arr.push_back(4);
/*	float s = arr.size();
	float max = *max_element(arr.begin(), arr.end());
	float min = *min_element(arr.begin(), arr.end());
	output << "size: " << s << " max: " << max   << " min: " << min << "\n";  */

	vector<float> arr2;
	arr2.push_back(0);
	arr2.push_back(5);
	arr2.push_back(5);
	arr2.push_back(10);
/*	float s2 = arr2.size();
	float max2 = *max_element(arr2.begin(), arr2.end());
	float min2 = *min_element(arr2.begin(), arr2.end());
	output << "size: " << s2 << " max: " << max2 << " min: " << min2 << "\n";*/

	vector<vector<float>> testData;
	testData.push_back(arr);
	testData.push_back(arr2);

	//float magic = testData.size(); //Magic number from number of vectors
	//get max/min value of container vector, rezise the value of axis
	//x depend on the number of vectors, y depends on max-min for each vector
	float startx = 0; 
	float endx = 1/float(testData.size());
	float starty = -1/float(testData.size());
	output << "ystart: " << starty <<  "\n";
	float endy = 1/float(testData.size());
	output << "endy: " << endy <<  "\n";

	for(int i=0; i < testData.size(); i++){
		vector<float> dataVector = testData[i];
		float max = *max_element(dataVector.begin(), dataVector.end());
		float min = *min_element(dataVector.begin(), dataVector.end());

		Vector2f parYstart = makeVector2f(startx,starty);
		Vector2f parYend = makeVector2f(startx,endy);

		viewer->addLine(parYstart, parYend, color, 2);

		startx+= endx*2;
	}
	startx = 0;
/*
	//parallel coordinates plot "axes"
	viewer->addLine(startX1, endX1, color, 2);
	viewer->addLine(startX2, endX2, color, 2);*/

	viewer->refresh();
}
