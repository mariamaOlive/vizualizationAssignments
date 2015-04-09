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

	//Rendering Properties
	float Alfa;
	float XScale;
	float YScale;

protected:
    GLGeometryViewer* viewer;

};                                            
                                             
#endif                                         
