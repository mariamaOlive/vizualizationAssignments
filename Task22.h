//-------------------------------------------- // 
#ifndef Task22H                    
#define Task22H                     
//-------------------------------------------- //
#include "Experiment.h"                        
#include "LinearAlgebra.h"             
#include "GLGeometryViewer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
//-------------------------------------------- //
                                            

///        First own experiment               //

class Task22 : public Experiment    
{                                              
    GEOX_CLASS(Task22)              
                                              
//Constructor / Destructor                     
public:                                       
    Task22();                       
    virtual ~Task22();              
                                             
//Methods                                      
public:                                       
    void CreateAxis();    
	void NegativeSlope();
	void CreateParaline(float x, float y);
	void LoadFile();
	void Paraline();
	float positionAxis(float value, float vMin, float vMax, float yMin, float yMax);

	virtual QWidget* createViewer();
                                               
//Attributes       
private:
	//fileName
	string fileName;

	//fileData 
	vector<string> variableName;
	vector<vector<float>> variableData;

	//scatterplot axis
	Vector2f scatterOrigin;
	Vector2f endAxisX;
	Vector2f endAxisY;
	Vector2f endAxisYDown;

	//parrallel plot axis
	Vector2f startX1;
	Vector2f endX1;
	Vector2f startX2;
	Vector2f endX2;

	int NumSamples;

	//Parameters for slope
	float slope;
	float Yorigo;

protected:
    GLGeometryViewer* viewer;

};                                            
                                             
#endif                                         
