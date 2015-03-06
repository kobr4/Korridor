#include <glm\glm.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtc\constants.hpp>
#include "ClosedSpaceGenerator.h"
#include <stdio.h>
#include "Texture.h"
#include "TextureGenerator.h"
#include <vector>



const float g_ground_level = -2.f;
unsigned int g_iteration_counter = 0;
unsigned int g_max_iteration = 20;

unsigned int * g_node_stack;
unsigned int g_node_position;
bool * g_maze_grid;
std::vector<T_TextureLightSource> gLightSourceVector;

void generateTop(T_QUAD * quadArray, unsigned int * quadArrayCount, unsigned int con_face_indice, float dimension, float unit_distance, float pos_x, float pos_y) {
	quadArray[*quadArrayCount].p1[0] = pos_x - unit_distance / 2.0f;
	quadArray[*quadArrayCount].p1[1] = unit_distance;
	quadArray[*quadArrayCount].p1[2] = pos_y - unit_distance / 2.0f;


	quadArray[*quadArrayCount].p2[0] = pos_x + unit_distance / 2.0f;
	quadArray[*quadArrayCount].p2[1] = unit_distance;
	quadArray[*quadArrayCount].p2[2] = pos_y - unit_distance / 2.0f;


	quadArray[*quadArrayCount].p3[0] = pos_x + unit_distance / 2.0f;
	quadArray[*quadArrayCount].p3[1] = unit_distance;
	quadArray[*quadArrayCount].p3[2] = pos_y + unit_distance / 2.0f;


	quadArray[*quadArrayCount].p4[0] = pos_x - unit_distance / 2.0f;
	quadArray[*quadArrayCount].p4[1] = unit_distance;
	quadArray[*quadArrayCount].p4[2] = pos_y + unit_distance / 2.0f;

		
	quadArray[*quadArrayCount].pos_x = pos_x;
	quadArray[*quadArrayCount].pos_y = pos_y;
	quadArray[*quadArrayCount].position = TOP;
	quadArray[*quadArrayCount].state = ENABLED;
	(*quadArrayCount)++;
}

void generateBottom(T_QUAD * quadArray, unsigned int * quadArrayCount, unsigned int con_face_indice, float dimension, float unit_distance, float pos_x, float pos_y) {
	quadArray[*quadArrayCount].p4[0] = pos_x - unit_distance / 2.0f;
	quadArray[*quadArrayCount].p4[1] = g_ground_level;
	quadArray[*quadArrayCount].p4[2] = pos_y - unit_distance / 2.0f;


	quadArray[*quadArrayCount].p3[0] = pos_x + unit_distance / 2.0f;
	quadArray[*quadArrayCount].p3[1] = g_ground_level;
	quadArray[*quadArrayCount].p3[2] = pos_y - unit_distance / 2.0f;


	quadArray[*quadArrayCount].p2[0] = pos_x + unit_distance / 2.0f;
	quadArray[*quadArrayCount].p2[1] = g_ground_level;
	quadArray[*quadArrayCount].p2[2] = pos_y + unit_distance / 2.0f;


	quadArray[*quadArrayCount].p1[0] = pos_x - unit_distance / 2.0f;
	quadArray[*quadArrayCount].p1[1] = g_ground_level;
	quadArray[*quadArrayCount].p1[2] = pos_y + unit_distance / 2.0f;

		
	quadArray[*quadArrayCount].pos_x = pos_x;
	quadArray[*quadArrayCount].pos_y = pos_y;
	quadArray[*quadArrayCount].position = BOTTOM;
	quadArray[*quadArrayCount].state = ENABLED;
	(*quadArrayCount)++;
}

bool vectorCompare(float * v1, float * v2,float epsilon) {
	if (((v1[0] < v2[0]+epsilon)&&(v1[0] > v2[0]-epsilon))
	&& ((v1[1] < v2[1]+epsilon)&&(v1[1] > v2[1]-epsilon))
	&& ((v1[2] < v2[2]+epsilon)&&(v1[2] > v2[2]-epsilon)))
	{
		return true;
	}
	return false;
}


bool belongToQuad(T_QUAD * quadArray1, float * p) {
	bool ret = false;
	
	if (vectorCompare(quadArray1->p1,p,0.1f)) {
		return true;
	}
	if (vectorCompare(quadArray1->p2,p,0.1f)) {
		return true;
	}
	if (vectorCompare(quadArray1->p3,p,0.1f)){
		return true;
	}
	if (vectorCompare(quadArray1->p4,p,0.1f)) {
		return true;
	}
	return ret;
}


bool quadIsDuplicated(T_QUAD * quadArray1, T_QUAD * quadArray2) {
	if (belongToQuad(quadArray1,quadArray2->p1) && 
		belongToQuad(quadArray1,quadArray2->p2) &&
		belongToQuad(quadArray1,quadArray2->p3) &&
		belongToQuad(quadArray1,quadArray2->p4)) {
		return true;
	}
}




void generateVertical(T_QUAD * quadArray, unsigned int * quadArrayCount, unsigned int con_face_indice, float dimension, float unit_distance, float pos_x, float pos_y, float angle) {
		
	
		quadArray[*quadArrayCount].p1[0] = - unit_distance / 2.0f;
		quadArray[*quadArrayCount].p1[1] = g_ground_level;
		quadArray[*quadArrayCount].p1[2] = unit_distance / 2.0f;


		quadArray[*quadArrayCount].p2[0] = - unit_distance / 2.0f;
		quadArray[*quadArrayCount].p2[1] = unit_distance;
		quadArray[*quadArrayCount].p2[2] = unit_distance / 2.0f;


		quadArray[*quadArrayCount].p3[0] = unit_distance / 2.0f;
		quadArray[*quadArrayCount].p3[1] = unit_distance;
		quadArray[*quadArrayCount].p3[2] = unit_distance / 2.0f;


		quadArray[*quadArrayCount].p4[0] = unit_distance / 2.0f;
		quadArray[*quadArrayCount].p4[1] = g_ground_level;
		quadArray[*quadArrayCount].p4[2] = unit_distance / 2.0f;

		
		
		if (angle != 0) {
			glm::vec3 p1(quadArray[*quadArrayCount].p1[0],quadArray[*quadArrayCount].p1[1],quadArray[*quadArrayCount].p1[2]);
			glm::vec3 p2(quadArray[*quadArrayCount].p2[0],quadArray[*quadArrayCount].p2[1],quadArray[*quadArrayCount].p2[2]);
			glm::vec3 p3(quadArray[*quadArrayCount].p3[0],quadArray[*quadArrayCount].p3[1],quadArray[*quadArrayCount].p3[2]);
			glm::vec3 p4(quadArray[*quadArrayCount].p4[0],quadArray[*quadArrayCount].p4[1],quadArray[*quadArrayCount].p4[2]);
			p1 = glm::rotateY(p1,angle);
			p2 = glm::rotateY(p2,angle);
			p3 = glm::rotateY(p3,angle);
			p4 = glm::rotateY(p4,angle);
			for (int i = 0;i < 3;i++) {
				quadArray[*quadArrayCount].p1[i] = p1[i];
				quadArray[*quadArrayCount].p2[i] = p2[i];
				quadArray[*quadArrayCount].p3[i] = p3[i];
				quadArray[*quadArrayCount].p4[i] = p4[i];
			}

		}
		

		quadArray[*quadArrayCount].p1[0] += pos_x;
		quadArray[*quadArrayCount].p1[2] += pos_y;

		quadArray[*quadArrayCount].p2[0] += pos_x;
		quadArray[*quadArrayCount].p2[2] += pos_y;

		quadArray[*quadArrayCount].p3[0] += pos_x;
		quadArray[*quadArrayCount].p3[2] += pos_y;

		quadArray[*quadArrayCount].p4[0] += pos_x;
		quadArray[*quadArrayCount].p4[2] += pos_y;

		quadArray[*quadArrayCount].pos_x = pos_x;
		quadArray[*quadArrayCount].pos_y = pos_y;

		bool bDisable = false;
		for (int i = 0;i < *quadArrayCount;i++) {
			if (quadArray[i].state == ENABLED && quadIsDuplicated(&quadArray[i],&quadArray[*quadArrayCount])) {
				//puts("Duplication detected.");
				quadArray[i].state = DISABLED;
				bDisable = true;
				break;
			}
		}


		quadArray[*quadArrayCount].position = VERTICAL;

		if (!bDisable) {
			quadArray[*quadArrayCount].state = ENABLED;
		} else {
			quadArray[*quadArrayCount].state = DISABLED;
		}
		(*quadArrayCount)++;
}

void generateVertical(T_QUAD * quadArray, unsigned int * quadArrayCount, unsigned int con_face_indice, float dimension, float unit_distance, float pos_x, float pos_y) {
	for (int i = 0; i < 4; i++) {
		generateVertical(quadArray,quadArrayCount,con_face_indice,dimension,unit_distance,pos_x,pos_y,i * 90);
	}
	
}

void generateMaze(T_QUAD * quadArray, unsigned int * quadArrayCount, unsigned int con_face_indice,float dimension, float unit_distance) {
	float nb_step = dimension / unit_distance;
	unsigned int ui_nb_step = (unsigned int)nb_step;
	for (int i = 0;i < nb_step;i++) {
		for (int j = 0;j < nb_step;j++) {
			float pos_x = i*nb_step;
			float pos_y = j*nb_step;

			unsigned int pos = i+j*nb_step;
			if (g_maze_grid[pos]) {
				generateBottom(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, pos_x, pos_y);
				generateTop(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, pos_x, pos_y);
				generateVertical(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, pos_x, pos_y);	
			}
			//printf("=> %d\n",*quadArrayCount);
		}
	}
}


void generateRoom(T_QUAD * quadArray, unsigned int * quadArrayCount, unsigned int con_face_indice,float dimension, float unit_distance) {
	float nb_step = dimension / unit_distance;
	unsigned int ui_nb_step = (unsigned int)nb_step;
	if (con_face_indice == -1) {
		float pos_x = rand()%ui_nb_step;
		float pos_y = rand()%ui_nb_step;

		//pos_x = 0.;f
		//pos_y = 0.f;
		//printf("Creating pos_x = %f, pos_y = %f \n",pos_x,pos_y);

		generateBottom(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, pos_x, pos_y);
		generateTop(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, pos_x, pos_y);
		generateVertical(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, pos_x, pos_y);
	} else {
		
		float new_pos_x = quadArray[con_face_indice].pos_x + (quadArray[con_face_indice].p1[0] - quadArray[con_face_indice].pos_x) + (quadArray[con_face_indice].p4[0] - quadArray[con_face_indice].pos_x);
		float new_pos_y = quadArray[con_face_indice].pos_y + (quadArray[con_face_indice].p1[2] - quadArray[con_face_indice].pos_y) + (quadArray[con_face_indice].p4[2] - quadArray[con_face_indice].pos_y);
		

		bool bCreate = true;
		for (int i = 0; i < *quadArrayCount;i++) {
			if (quadArray[i].pos_x == new_pos_x && quadArray[i].pos_y == new_pos_y) {
				bCreate = false;
				break;
			}
		}

		if (bCreate) {
			//quadArray[con_face_indice].state = DISABLED;
			//printf("Creating pos_x = %f, pos_y = %f indice=%d quadcount=%d\n",new_pos_x,new_pos_y,con_face_indice,*quadArrayCount);
			g_iteration_counter++;
			generateBottom(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, new_pos_x, new_pos_y);
			generateTop(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, new_pos_x, new_pos_y);
			generateVertical(quadArray,quadArrayCount,con_face_indice, dimension, unit_distance, new_pos_x, new_pos_y);
		}
	}

	
	
	if (g_iteration_counter < g_max_iteration) {
		unsigned int indice = rand()%(*quadArrayCount);
		while (quadArray[indice].state != ENABLED && quadArray[indice].position != VERTICAL) {
			indice = rand()%(*quadArrayCount);
		}
		generateRoom(quadArray, quadArrayCount, indice, dimension, unit_distance);
	}
}


void generateLightSource(T_QUAD * quadArray,unsigned int quadArrayCount,int begin, int end, T_SPACE_OBJECT * spaceArray) {
	unsigned int quadBegin = 0;
	unsigned int quadEnd = 0;
	int quadCounter = 0;
	T_TextureLightSource lightSource;

	unsigned int counter = 0;
	
	for (unsigned int i = 0;i < quadArrayCount;i++){ 
		if (quadArray[i].state == ENABLED) {
			
			if (quadCounter == begin) {
				quadBegin = i;
			}

			if (quadCounter == end) {
				quadEnd = i;
			}

			quadCounter++;
		}
	}
	if (quadEnd == 0) {
		quadEnd = quadArrayCount;
	}	
	
	for (unsigned int i = quadBegin;i < quadEnd;i++) {
		if (quadArray[i].state == ENABLED) {
			counter++;
		}
	}

	// Lightsource generation
	unsigned int nbLightSource = counter / 15;
	//nbLightSource = 1;
	spaceArray->lightCount = nbLightSource;
	if (nbLightSource > 0) {
		spaceArray->lightPosition = (float*)malloc(sizeof(float)*3*nbLightSource);
		for (int nlt = 0; nlt < nbLightSource;nlt++) {
			unsigned int pos = 0;
			do {
				pos = (rand()%(quadEnd - quadBegin)) + quadBegin;
				if ((quadArray[pos].state == ENABLED)&&(quadArray[pos].position==TOP)) {
					float x = (quadArray[pos].p1[0] + quadArray[pos].p2[0] + quadArray[pos].p3[0] + quadArray[pos].p4[0])/4.0f;
					float y = quadArray[pos].p1[1] - 0.5f;
					float z = (quadArray[pos].p1[2] + quadArray[pos].p2[2] + quadArray[pos].p3[2] + quadArray[pos].p4[2])/4.0f;
					spaceArray->lightPosition[nlt * 3] = x;
					spaceArray->lightPosition[nlt * 3 + 1] = y;
					spaceArray->lightPosition[nlt * 3 + 2] = z;


					lightSource.color[0] = 1.0f;
					lightSource.color[1] = 1.0f;
					lightSource.color[2] = 1.0f;
					lightSource.position[0] = x;
					lightSource.position[1] = y;
					lightSource.position[2] = z;

					gLightSourceVector.push_back(lightSource);
					
				}
			} while ((quadArray[pos].state != ENABLED)||(quadArray[pos].position!=TOP));
		}	
	}
}

void quadArrayToVertexbuffer(T_QUAD * quadArray,unsigned int quadArrayCount, int begin, int end,unsigned int nbTexturePerLine, T_SPACE_OBJECT * spaceArray) {
	int quadCounter = 0;
	unsigned int quadBegin = 0;
	unsigned int quadEnd = 0;

	T_TextureLightSource lightSource;
	
	T_TextureQuad currentQuad;

	/*
	unsigned int nbLightSource = 4;
	lightSource[0].color[0] = 1.0f;
	lightSource[0].color[1] = 1.0f;
	lightSource[0].color[2] = 1.0f;

	lightSource[0].position[0] = 22.0f;
	lightSource[0].position[1] = 4.0f;
	lightSource[0].position[2] = 61.0f;

	lightSource[1].color[0] = 1.0f;
	lightSource[1].color[1] = 1.0f;
	lightSource[1].color[2] = 1.0f;

	lightSource[1].position[0] = 40.0f;
	lightSource[1].position[1] = 4.0f;
	lightSource[1].position[2] = 80.0f;

	lightSource[2].color[0] = 1.0f;
	lightSource[2].color[1] = 1.0f;
	lightSource[2].color[2] = 1.0f;

	lightSource[2].position[0] = 60.0f;
	lightSource[2].position[1] = 4.0f;
	lightSource[2].position[2] = 50.0f;

	lightSource[3].color[0] = 1.0f;
	lightSource[3].color[1] = 1.0f;
	lightSource[3].color[2] = 1.0f;

	lightSource[3].position[0] = 80.0f;
	lightSource[3].position[1] = 4.0f;
	lightSource[3].position[2] = 46.0f;
	*/


	for (unsigned int i = 0;i < quadArrayCount;i++){ 
		if (quadArray[i].state == ENABLED) {
			
			if (quadCounter == begin) {
				quadBegin = i;
			}

			if (quadCounter == end) {
				quadEnd = i;
			}

			quadCounter++;
		}
	}
	if (quadEnd == 0) {
		quadEnd = quadArrayCount;
	}


	spaceArray->triangleCount = 0;

	for (unsigned int i = quadBegin;i < quadEnd;i++) {
		if (quadArray[i].state == ENABLED) {
			spaceArray->triangleCount += 2;
		}
	}

	spaceArray->triangleArray = (float*)malloc(sizeof(float) * 3 * 5 * spaceArray->triangleCount);
	
	Texture * main_texture = new Texture(1024,1024);
	Texture * lightmap_texture = new Texture(256,256);
	int tmpTCount = 0;
	quadCounter = 0;
	for (unsigned int i = quadBegin;i < quadEnd;i++) {
		if (quadArray[i].state == ENABLED) {
			float xBottom = (float)(quadCounter % nbTexturePerLine) / (float)nbTexturePerLine;
			float yBottom = (float)(quadCounter / nbTexturePerLine) / (float)nbTexturePerLine;
			float xTop = xBottom + 1.f / (float)nbTexturePerLine;
			float yTop = yBottom + 1.f / (float)nbTexturePerLine;
			Texture * element_texture = NULL;
			if (quadArray[i].position == VERTICAL) {
				element_texture = TextureGenerator::generateTileTexture(256,256,10,8,16,1,130,30,30,100,100,100);

			} else {
				element_texture = TextureGenerator::generateTileTexture(256,256,0,16,256,1,100,50,0,10,10,10);
			}
			//printf("Packing texture at %d : %d %d\n",1024 / nbTexturePerLine * (quadCounter % nbTexturePerLine), 1024 / nbTexturePerLine * (quadCounter / nbTexturePerLine),nbTexturePerLine);
			//printf("xBottom=%f yBottom=%f xTop=%f yTop=%f\n",xBottom,yBottom,xTop,yTop);
			main_texture->packTexture(element_texture,1024 / nbTexturePerLine * (quadCounter % nbTexturePerLine),1024 / nbTexturePerLine * (quadCounter / nbTexturePerLine));
			delete element_texture;

			for (int z = 0;z < 3;z++) {
				currentQuad.p1[z] = quadArray[i].p1[z];
				currentQuad.p2[z] = quadArray[i].p2[z];
				currentQuad.p3[z] = quadArray[i].p3[z];
				currentQuad.p4[z] = quadArray[i].p4[z];
			}
			//Texture * element_lightmap = TextureGenerator::generateLightmapTexture(256,256,&lightSource,&currentQuad);
			
			
			// Light computation starts here.
			Texture * element_lightmap = TextureGenerator::generateFloorLightmap(64,64,0);
			for (int l = 0;l < gLightSourceVector.size();l++) {
			//for (int l = 0;l < 1;l++) {
				/*
				lightSource.color[0] = 1.0f;
				lightSource.color[1] = 1.0f;
				lightSource.color[2] = 1.0f;

				lightSource.position[0] = spaceArray->lightPosition[l * 3];
				lightSource.position[1] = spaceArray->lightPosition[l * 3 + 1];
				lightSource.position[2] = spaceArray->lightPosition[l * 3 + 2];
				*/
				lightSource = gLightSourceVector[l];

				/*
				lightSource.color[0] = 1.0f;
				lightSource.color[1] = 1.0f;
				lightSource.color[2] = 1.0f;
				lightSource.position[0] = 22.0f;
				lightSource.position[1] = 4.0f;
				lightSource.position[2] = 61.0f;
				*/
				//printf("Light[%d] %f %f %f\n",l,lightSource.position[0],lightSource.position[1],lightSource.position[2]);
				glm::vec3 light_position = glm::vec3(lightSource.position[0],lightSource.position[1],lightSource.position[2]);
				for (unsigned int i = 0;i < quadArrayCount;i++){ 
					if (quadArray[i].state == ENABLED) {
						quadArray[i].distance = glm::distance(light_position,glm::vec3(quadArray[i].p1[0],quadArray[i].p1[1],quadArray[i].p1[2]));
			
						float d;
						d = glm::distance(light_position,glm::vec3(quadArray[i].p2[0],quadArray[i].p2[1],quadArray[i].p2[2]));
						if (d < quadArray[i].distance) {
							quadArray[i].distance = d;
						}
						d = glm::distance(light_position,glm::vec3(quadArray[i].p3[0],quadArray[i].p3[1],quadArray[i].p3[2]));
						if (d < quadArray[i].distance) {
							quadArray[i].distance = d;
						}
						d = glm::distance(light_position,glm::vec3(quadArray[i].p4[0],quadArray[i].p4[1],quadArray[i].p4[2]));
						if (d < quadArray[i].distance) {
							quadArray[i].distance = d;
						}
			
					}
				}
				Texture * element_lightmap_per_light;
				if (quadArray[i].distance < 40.0f) {	
					element_lightmap_per_light = TextureGenerator::generateLightmapTextureWithOcclusion(64,64,&lightSource,&currentQuad,quadArray,quadArrayCount,i);
				
				} else {
					element_lightmap_per_light = TextureGenerator::generateFloorLightmap(64,64,30);
				}

				element_lightmap->merge(element_lightmap_per_light);
				delete element_lightmap_per_light;
				
			}
			element_lightmap->blur();
			lightmap_texture->packTexture(element_lightmap,256 / nbTexturePerLine * (quadCounter % nbTexturePerLine),256 / nbTexturePerLine * (quadCounter / nbTexturePerLine));
			delete element_lightmap;
			
			// Light computation ends here.


			quadCounter++;
			spaceArray->triangleArray[tmpTCount * 5 + 0] = quadArray[i].p1[0];
			spaceArray->triangleArray[tmpTCount * 5 + 1] = quadArray[i].p1[1];
			spaceArray->triangleArray[tmpTCount * 5 + 2] = quadArray[i].p1[2];

			spaceArray->triangleArray[tmpTCount * 5 + 3] = xBottom;
			spaceArray->triangleArray[tmpTCount * 5 + 4] = yBottom;
			tmpTCount++;
			spaceArray->triangleArray[tmpTCount * 5 + 0] = quadArray[i].p2[0];
			spaceArray->triangleArray[tmpTCount * 5 + 1] = quadArray[i].p2[1];
			spaceArray->triangleArray[tmpTCount * 5 + 2] = quadArray[i].p2[2];

			spaceArray->triangleArray[tmpTCount * 5 + 3] = xTop;
			spaceArray->triangleArray[tmpTCount * 5 + 4] = yBottom;
			tmpTCount++;
			spaceArray->triangleArray[tmpTCount * 5 + 0] = quadArray[i].p3[0];
			spaceArray->triangleArray[tmpTCount * 5 + 1] = quadArray[i].p3[1];
			spaceArray->triangleArray[tmpTCount * 5 + 2] = quadArray[i].p3[2];

			spaceArray->triangleArray[tmpTCount * 5 + 3] = xTop;
			spaceArray->triangleArray[tmpTCount * 5 + 4] = yTop;
			tmpTCount++;
			spaceArray->triangleArray[tmpTCount * 5 + 0] = quadArray[i].p3[0];
			spaceArray->triangleArray[tmpTCount * 5 + 1] = quadArray[i].p3[1];
			spaceArray->triangleArray[tmpTCount * 5 + 2] = quadArray[i].p3[2];

			spaceArray->triangleArray[tmpTCount * 5 + 3] = xTop;
			spaceArray->triangleArray[tmpTCount * 5 + 4] = yTop;
			tmpTCount++;
			spaceArray->triangleArray[tmpTCount * 5 + 0] = quadArray[i].p4[0];
			spaceArray->triangleArray[tmpTCount * 5 + 1] = quadArray[i].p4[1];
			spaceArray->triangleArray[tmpTCount * 5 + 2] = quadArray[i].p4[2];

			spaceArray->triangleArray[tmpTCount * 5 + 3] = xBottom;
			spaceArray->triangleArray[tmpTCount * 5 + 4] = yTop;
			tmpTCount++;
			spaceArray->triangleArray[tmpTCount * 5 + 0] = quadArray[i].p1[0];
			spaceArray->triangleArray[tmpTCount * 5 + 1] = quadArray[i].p1[1];
			spaceArray->triangleArray[tmpTCount * 5 + 2] = quadArray[i].p1[2];

			spaceArray->triangleArray[tmpTCount * 5 + 3] = xBottom;
			spaceArray->triangleArray[tmpTCount * 5 + 4] = yBottom;
			tmpTCount++;			
		}
	}

	spaceArray->texture = main_texture;
	spaceArray->lightMapTexture = lightmap_texture;
}

unsigned int checkMaze(unsigned int position,unsigned int linesize) {
	int count = 0;
	unsigned int x = position % linesize;
	unsigned int y = position / linesize;

	if (y > 0)
	if (!g_maze_grid[position-linesize]) {
		count++;
	}
	if (x < linesize-1)
	if (!g_maze_grid[position+1]) {
		count++;
	}
	if (y < linesize - 1)
	if (!g_maze_grid[position+linesize]) {
		count++;
	}
	if (x > 0) 
	if (!g_maze_grid[position-1]) {
		count++;
	}
	return count;
}


unsigned int hasRoomToDig(unsigned int position,unsigned int linesize) {
	unsigned int x = position % linesize;
	unsigned int y = position / linesize;
	bool vmap[4];
	bool hasExit = false;
	for (int i = 0;i < 4;i++) {
		vmap[i] = false;
	}
	if (y > 0) {
		if (g_maze_grid[position-linesize] == false) {
			if (checkMaze(position-linesize,linesize) >= 3) {
				//return position-linesize;
				vmap[0] = true;
				hasExit = true;
			}
		}
	}
	if (y < linesize - 1) {
		if (g_maze_grid[position+linesize] == false) {
			if (checkMaze(position+linesize,linesize) >= 3) {
				//return position+linesize;
				vmap[1] = true;
				hasExit = true;
			}
		}
	}

	if (x < linesize-1) {
		if (g_maze_grid[position+1] == false) {
			if (checkMaze(position+1,linesize) >= 3) {
				//return position+1;
				vmap[2] = true;
				hasExit = true;
			}
		}
	}
	if (x > 0) {
		if (g_maze_grid[position-1] == false) {
			if (checkMaze(position-1,linesize) >= 3) {
				//return position-1;
				vmap[3] = true;
				hasExit = true;
			}
		}
	}

	if (hasExit) {
		int i;
		while (vmap[i = rand()%4] == false);
		switch (i) {
			case 0 : return position-linesize; 
			case 1 : return position+linesize;
			case 2 : return position+1;
			case 3 : return position-1;
		}
	}

	return 0;
}

void digRoom(unsigned int position,unsigned int linesize) {
	unsigned int newpos;
	g_maze_grid[position] = true;

	if (newpos = hasRoomToDig(position,linesize)) {
		g_node_stack[g_node_position] = newpos;
		g_node_position++;
		//printf("DIG! %d %d\n",g_node_position,newpos);
		digRoom(newpos,linesize);

	} else if (g_node_position > 0){
		g_node_position--;
		digRoom(g_node_stack[g_node_position],linesize);
	}
}

void ClosedSpaceGenerator::generateSpace(float dimension, float unit_distance, T_SPACE_OBJECT ** spaceArray, unsigned int * spaceCount,unsigned int maxTextureWidth, unsigned int textureWidth) {
	float step_count = dimension / unit_distance;
	unsigned int istep_count = (unsigned int)step_count;
	unsigned int quadArrayCount = 0;
	unsigned int enabledQuadCounter = 0;
	T_QUAD * quadArray = (T_QUAD*)malloc(sizeof(T_QUAD) *istep_count * istep_count* 6);
	for (int i = 0;i < istep_count*istep_count*6;i++) {
		quadArray[i].state = DISABLED;
	}
	
	g_node_stack = new unsigned int[istep_count*istep_count];
	for (int i = 0;i < istep_count*istep_count;i++) {
		g_node_stack[i] = 0;
	}
	g_node_position = 0;
	g_maze_grid = new bool[istep_count * istep_count];
	for (int i = 0;i < istep_count * istep_count;i++) {
		g_maze_grid[i] = false;
	}
	//printf("nb_step=%d\n",istep_count);
	
	digRoom(50,istep_count);
	
	generateMaze(quadArray,&quadArrayCount,-1,dimension, unit_distance);

	//generateRoom(quadArray,&quadArrayCount,-1,dimension, unit_distance);
	

	for (int i = 0;i < quadArrayCount;i++) {
		if (quadArray[i].state == ENABLED) {
			enabledQuadCounter++;
		}
	}

	unsigned int step = (maxTextureWidth / textureWidth)*(maxTextureWidth / textureWidth);
	unsigned int objectCount = enabledQuadCounter / step + 1;
	*spaceArray = (T_SPACE_OBJECT*)malloc(sizeof(T_SPACE_OBJECT)*objectCount);
	*spaceCount = objectCount;
	
	for (int i = 0; i < objectCount;i++) {
		generateLightSource(quadArray,quadArrayCount,i * step, (i+1) * step > enabledQuadCounter ? enabledQuadCounter : (i+1) * step , &(*spaceArray)[i]); 
	}
	

	for (int i = 0; i < objectCount;i++) {
		//printf("Creating quad array indices %d to %d\n",i*step,(i+1) * step > enabledQuadCounter ? enabledQuadCounter : (i+1) * step);
		quadArrayToVertexbuffer(quadArray, quadArrayCount, i * step, (i+1) * step > enabledQuadCounter ? enabledQuadCounter : (i+1) * step ,maxTextureWidth/textureWidth,&(*spaceArray)[i]);
	}
	free(quadArray);
}

void ClosedSpaceGenerator::generateSphere(T_SPACE_OBJECT * sphere, float radius, unsigned int nbSlices) {
	sphere->triangleCount = nbSlices * nbSlices * 2;
	sphere->triangleArray = (float*)malloc(sizeof(float) * sphere->triangleCount  * 3 * 5);
	unsigned int count = 0;
	for (int i = 0;i < nbSlices;i++) {
		for (int j = 0;j < nbSlices;j++) {

			float alpha = glm::pi<float>()*2 / (nbSlices+1) * i;
            float beta = glm::pi<float>()*2 / nbSlices * j;
            float alpha1 = glm::pi<float>()*2  / (nbSlices+1) * (i + 1);
            float beta1 = glm::pi<float>()*2 / nbSlices * (j + 1);


			glm::vec3 a = glm::vec3(radius * sin(alpha) * cos(beta), radius * cos(alpha),radius * sin(alpha) * sin(beta));
            glm::vec3 b = glm::vec3(radius * sin(alpha) * cos(beta1), radius * cos(alpha),radius * sin(alpha) * sin(beta1));
            glm::vec3 c = glm::vec3(radius * sin(alpha1) * cos(beta1), radius * cos(alpha1),radius * sin(alpha1) * sin(beta1));
            glm::vec3 d = glm::vec3(radius * sin(alpha1) * cos(beta), radius * cos(alpha1),radius * sin(alpha1) * sin(beta));

			for (int k = 0;k < 3;k++) sphere->triangleArray[count++] = a[k];
			count += 2;
			for (int k = 0;k < 3;k++) sphere->triangleArray[count++] = b[k];
			count += 2;
			for (int k = 0;k < 3;k++) sphere->triangleArray[count++] = c[k];
			count += 2;

			for (int k = 0;k < 3;k++) sphere->triangleArray[count++] = a[k];
			count += 2;
			for (int k = 0;k < 3;k++) sphere->triangleArray[count++] = c[k];
			count += 2;			
			for (int k = 0;k < 3;k++) sphere->triangleArray[count++] = d[k];
			count += 2;
		}
	}

	sphere->texture = TextureGenerator::generateWhiteTexture();
}