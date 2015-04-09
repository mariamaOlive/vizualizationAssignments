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
    

///        Second part of Task2               //

                                          
IMPLEMENT_GEOX_CLASS( Task22, 0 )    
{                                               
	BEGIN_CLASS_INIT( Task22 );       
	ADD_NOARGS_METHOD(Task22::LoadFile)
	ADD_NOARGS_METHOD(Task22::Paraline)
	ADD_SEPARATOR("File Name")	
	ADD_STRING_PROP(fileName,0)
	ADD_SEPARATOR("Visual Scaling")
	ADD_FLOAT32_PROP(XScale,0)
	ADD_FLOAT32_PROP(YScale,0)
	ADD_SEPARATOR("Rendering")
	ADD_FLOAT32_PROP(Alfa,0)
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
	fileName="Cars.csv";
	Alfa=1;
	XScale=1;
	YScale=1;

	//Data from the file
	//vector<string> variableName;
	//vector<vector<float>> variableData;
}                                               
                                              
Task22::~Task22()        
{             
}   

void Task22::LoadFile()
{
	//Clear the previous data from the vector
	variableName.clear();
	variableData.clear();

	//Opening and extracting info from the file
	ifstream file;
	file.open(fileName);
	vector<string> dataString;

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
		int numberOfVariable= variableName.size();
		bool firstTime = true;

		for(int i=1;i<dataString.size() ;i++){
			//starts from the second element because the first line is the name of the variables
			string analizedLine= dataString[i];
			commaPosition=analizedLine.find_first_of(',',0);
			
			int auxPos=0;

			while(commaPosition!= std::string::npos){
				commaPosition=analizedLine.find_first_of(',',0);
				float valueVariable;
				valueVariable=stof(analizedLine.substr(0, commaPosition));

				//Tranforming the read string value into float and putting into the array
				if(firstTime){
					//It just enters here the first time that in order to create the vector of the variables
					vector<float> auxArray;
					auxArray.push_back(valueVariable);
					variableData.push_back(auxArray);
				}else{
					variableData.at(auxPos).push_back(valueVariable);
					auxPos++;
				}
				//Removing the recognized variable from the string
				analizedLine.erase(0,commaPosition+1);
			}
			firstTime=false;
		}
	}

}                                             

void Task22::Paraline(){
	viewer->clear();
	Vector4f color = makeVector4f(1,1,1,1);
	Vector4f color2 = makeVector4f(0,1,1,Alfa);

	//get max/min value of container vector, rezise the value of axis
	//x depend on the number of vectors, y depends on max-min for each vector

	float startx = 0; 
	float endx = 1/float(variableData.size())*XScale;
	float starty = -1/float(variableData.size())*YScale;
	float endy = 1/float(variableData.size())*YScale;

	/////////////////////////////////////////////////////////////

	for(int i=1; i < variableData.size(); i++){

		//Drawing parallel axis
		Vector2f parYstart = makeVector2f(startx,starty);
		Vector2f parYend = makeVector2f(startx,endy);
		viewer->addLine(parYstart, parYend, color, 2);
		
		//Calculating min and max values for previous vector
		vector<float> dataVector = variableData[i-1];
		float max = *max_element(dataVector.begin(), dataVector.end());
		float min = *min_element(dataVector.begin(), dataVector.end());

		//Calculating min and max for this vector
		vector<float> dataVector2 = variableData[i];
		float max2 = *max_element(dataVector2.begin(), dataVector2.end());
		float min2 = *min_element(dataVector2.begin(), dataVector2.end());

		//Drawing lines between the point
		for(int j=0; j<dataVector2.size(); j++){ 
			float value1 = dataVector[j];
			//Calculating the position of the fist variable
			float new1 = positionAxis(value1, max, min, endy, starty);

			float value2 = dataVector2[j];
			//Calculating the position of the next variable
			float new2 = positionAxis(value2, max2, min2, endy, starty);

			Vector2f lineStart = makeVector2f(startx,new1);
			Vector2f lineEnd = makeVector2f(startx+endx*2,new2);
			viewer->addLine(lineStart, lineEnd, color2, 2);
		}
		//iterating to next parallel axis (X value)
		startx+= endx*2;
	}

	//Drawing last parallel axis
	Vector2f parYstart = makeVector2f(startx,starty);
	Vector2f parYend = makeVector2f(startx,endy);
	viewer->addLine(parYstart, parYend, color, 2);

	viewer->refresh();
}

float Task22::positionAxis(float value, float vMin, float vMax, float yMin, float yMax){
	float finalValue = 0;
	//Calculating the proporcional value in the axis
	//float v = vMax-vMin
	if(vMax-vMin != 0)
		finalValue= ((value-vMin)*(yMax-yMin)/(vMax-vMin))+yMin;
	return finalValue;
}
