//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task32.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"

IMPLEMENT_GEOX_CLASS( Task32, 0)
{
    BEGIN_CLASS_INIT( Task32 );

    ADD_SEPARATOR("Scalarfield")
    ADD_STRING_PROP(ScalarfieldFilename, 0)
	ADD_FLOAT32_PROP(isovalue,0)
    ADD_NOARGS_METHOD(Task32::DrawScalarField)
}

QWidget* Task32::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}


Task32::Task32()
{
    viewer = NULL;
    ScalarfieldFilename = "SimpleGrid.am";
	isovalue = 1;
}

Task32::~Task32() {}

void Task32::DrawGrid(ScalarField2 field){
	Vector2f minBox= makeVector2f(field.boundMin()[0], field.boundMin()[1]);
	Vector2f maxBox= makeVector2f(field.boundMax()[0], field.boundMax()[1]);

	//Adding boundaries of the grid (boundBox)
	viewer->addLine(makeVector2f(minBox[0],minBox[1]), makeVector2f(maxBox[0],minBox[1]));
	viewer->addLine(makeVector2f(maxBox[0],minBox[1]), makeVector2f(maxBox[0],maxBox[1]));
	viewer->addLine(makeVector2f(maxBox[0],maxBox[1]), makeVector2f(minBox[0],maxBox[1]));
	viewer->addLine(makeVector2f(minBox[0],maxBox[1]), makeVector2f(minBox[0],minBox[1]));

	//Adding grid line horizontally
	for(int i=1;i<field.dims()[1]-1;i++){
		Vector2f p1= field.nodePosition(0,i);	
		Vector2f p2= field.nodePosition(field.dims()[0]-1,i);
	
		viewer->addLine(p1,p2);
	}

	//Adding grid line vertically
	for(int i=1;i<field.dims()[0]-1;i++){
		Vector2f p1= field.nodePosition(i, 0);	
		Vector2f p2= field.nodePosition(i,field.dims()[1]-1);
	
		viewer->addLine(p1,p2);
	}

    viewer->refresh();
}

void Task32::DrawScalarField()
{
    viewer->clear();

    //Load scalar field
    ScalarField2 field;
    if (!field.load(ScalarfieldFilename))
    {
        output << "Error loading field file " << ScalarfieldFilename << "\n";
        return;
    }

	DrawGrid(field);

    //Get the minimum/maximum value in that field
    /*float32 min = std::numeric_limits<float32>::max();
    float32 max = -std::numeric_limits<float32>::max();
    for(size_t j=0; j<field.dims()[1]; j++)
    {
        for(size_t i=0; i< field.dims()[0]; i++)
        {
            const float32 val = field.nodeScalar(i,j);
            min = val < min ? val : min;
            max = val > max ? val : max;
        }
    }

    //Draw a point for each grid vertex.
    for(size_t j=0; j<field.dims()[1]; j++)
    {
        for(size_t i=0; i<field.dims()[0]; i++)
        {
            const float32 val = field.nodeScalar(i, j);
            const float32 c = (val - min) / (max - min);

            Point2D p;
            p.position  = field.nodePosition(i, j);
            p.size = 5;
            //Use a grayscale depending on the actual value
            p.color[0] = c; p.color[1] = c; p.color[2] = c;
            viewer->addPoint(p);
        }
    }*/

	//Loading cell into a vector

	for(int i=0; i<field.dims()[0]-1;i++){		
		for(int j=0; j<field.dims()[1]-1; j++){
			Cell1 cell;
			//loading the values of each point
			cell.v1=field.nodeScalar(i,j);
			cell.v2=field.nodeScalar(i+1,j);
			cell.v3=field.nodeScalar(i+1,j+1);
			cell.v4=field.nodeScalar(i,j+1);
			//loading the position of each point
			cell.p1=field.nodePosition(i,j);
			cell.p2=field.nodePosition(i+1,j);
			cell.p3=field.nodePosition(i+1,j+1);
			cell.p4=field.nodePosition(i,j+1);
			vectorCells.push_back(cell);
		}
	}

	output <<"\n";

	//for each cell, get value for each of the four points and compare them to isovalue, (true/false smaller or bigger?)
	// if all are smaller/bigger do nothing, if one point is diffrent, if two point are diffrent (check for diagonal case)

	//Going through each cell and checking if there are isoline intersecting
	for(int i=0; i<vectorCells.size();i++){
			Cell1 cell = vectorCells[i];
			bool sign[4];
			
			//Comparing each vertex to the isovalue
			
				sign[0] =  cell.v1>=isovalue ? true:false ;
				sign[1] =  cell.v2>=isovalue ? true:false ;
				sign[2] =  cell.v3>=isovalue ? true:false ;
				sign[3] =  cell.v4>=isovalue ? true:false ;
			
			//output << sign[0] << " " << sign[1] << " " << sign[2] << " " << sign[3] <<"\n" <<"\n";

			//Checking if the isovalue intersects the cell
			//if at least one of the vertex is different there's a isoline in the cell
			if(!(sign[0] == sign[1] && sign[0] == sign[2] && sign[0] == sign[3] 
					&& sign[1] == sign[2] && sign[1] == sign[3] && sign[2] == sign[3])){
				
				//if it is a diagonal case
				if(sign[0]==sign[2] && sign[1]==sign[3]){ 
					output << "Diagonal case Cellnumber: " << i << "\n" << "\n";
				}

				else{
				//if it is the other two cases
					Vector2f pInter [2]; 
					int count=0;

					if(sign[0]!=sign[1]){
						pInter[count]=makeVector2f(calculateIntersection(cell.v1, cell.v2, cell.p1[0], cell.p2[0]), cell.p1[1]);
						//output << "down " << pInter[count] << "\n";
						count++;
					}

					if(sign[1]!=sign[2]){
						pInter[count]=makeVector2f(cell.p2[0], calculateIntersection(cell.v3, cell.v2, cell.p3[1], cell.p2[1]));					
						//output << "left" << pInter[count] << "\n";
						count++;
					}

					if(sign[2]!=sign[3]){
						pInter[count]=makeVector2f(calculateIntersection(cell.v4, cell.v3, cell.p4[0], cell.p3[0]), cell.p3[1]);			
						//output << "up" << pInter[count] << "\n";
						count++;
					}
					
					if(sign[3]!=sign[0]){
						pInter[count]=makeVector2f(cell.p4[0], calculateIntersection(cell.v4, cell.v1, cell.p4[1], cell.p1[1]));					
						//output << "right" << pInter[count] << "\n";
					}

					viewer->addLine(pInter[0], pInter[1]);
				}

			}
			viewer->refresh();
	}
}

//reversed linear interpolation (in 1D domain)
float Task32::calculateIntersection(float vMin, float vMax, float xMin, float xMax){
	//float unknownCoord = isovalue*((xMax-xMin)/(vMax-vMin))+xMin;
	float unknownCoord = (isovalue*(xMax-xMin)-vMin*xMax+vMax*xMin)/(vMax-vMin);

	return unknownCoord;
}

