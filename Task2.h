//-------------------------------------------- // 
#ifndef Task2H                    
#define Task2H                     
//-------------------------------------------- //
#include "Experiment.h"                        
#include "LinearAlgebra.h"             
#include "GLGeometryViewer.h"
//-------------------------------------------- //
                                            

///        Task2: Part1               //
///		Group members: Terese Nothnagel, Mariama Oliveira, Lui Pillmann de Barros	//

class Task2 : public Experiment    
{                                              
    GEOX_CLASS(Task2)              
                                              
//Constructor / Destructor                     
public:                                       
    Task2();                       
    virtual ~Task2();              
                                             
//Methods                                      
public:                                       
    void CreateAxis();    
	void NegativeSlope();
	void CreateParaline(float x, float y);
	void DrawCircle();
	void DrawHyperbola();

	virtual QWidget* createViewer();
                                               
//Attributes       
private:
	//scatterplot axis
	Vector2f scatterOrigin;
	Vector2f endAxisX;
	Vector2f endAxisY;
	Vector2f endAxisYDown;
	Vector2f endAxisXLeft;

	//parrallel plot axis
	Vector2f startX1;
	Vector2f endX1;
	Vector2f startX2;
	Vector2f endX2;

	int NumSamples;

	//Parameters for slope
	float slope;
	float Yorigo;

	//Parametrs for the circle
	float Radius;
	Vector2f Center;

	//Parameters for the hypobola
	float a;
	float b;
	float minu;
	float maxu;

protected:
    GLGeometryViewer* viewer;

};                                            
                                             
#endif                                         
