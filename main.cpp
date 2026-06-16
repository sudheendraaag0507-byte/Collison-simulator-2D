#include "include\boilerPlate.h"
#include "include\Recorder.h"

using namespace std;

class general;
class particle;
class grid;

// here is where i store all the info related to the simulation grid
class general {
protected:
    static int numberOfParticle;
    static float maxVelocity;
    static float maxRadius;
    static float maxX, maxY, maxZ;
    static int maxDivison;
    static float smallest_radii;
    static grid* cell;
    static particle* particles;

public:
    

    void data(int n, float Vel, float Radius, float X, float Y, float Z);

    static particle* particleArray();

    // this the radii math which checks whether the particles are actually hitting even though when the cells overlap

    static bool radiiMath(vec3 position1, vec3 position2, float radius1, float radius2) {
        float sqdis = (position1.x - position2.x) * (position1.x - position2.x) + (position1.y - position2.y) * (position1.y - position2.y);

        if (sqdis <= (radius1 + radius2) * (radius1 + radius2)) {
            vec3 Prel = position1 - position2;

            //cout << Prel.x << " " << Prel.y << " " << length(Prel) << "<-----this is while hitting \n"; // this for debugging in case of any failure in future
            return true;
        }
        else{ 
            vec3 Prel = position1 - position2;

            //cout << Prel.x << " " << Prel.y << " " << "<-----this is before or after hitting \n";
        
            return false;
        }
    }

    // this is the actual 2D collision math

    //conservation of linear moment is being used and here the velocities are passed by reference
    static void collisionMath(float ds , float m1, float m2, float* v1, float* v2, vec3 position1, vec3 position2) {
        if(ds <= 0){
            
            float theta = atan2((position1.y - position2.y), (position1.x - position2.x));
            float cos = cosf(theta);
            float sin = sinf(theta);

            // the math is being done along the line of approach

            float old_v1x = v1[0] * cos + v1[1] * sin;
            float old_v2x = v2[0] * cos + v2[1] * sin;
            float old_v1y = v1[1] * cos - v1[0] * sin;
            float old_v2y = v2[1] * cos - v2[0] * sin;

            float new_v1x = ((m1 - m2) * old_v1x + 2 * m2 * old_v2x) / (m1 + m2);
            float new_v2x = (2 * m1 * old_v1x + (m2 - m1) * old_v2x) / (m1 + m2);
            float new_v1y = old_v1y;
            float new_v2y = old_v2y;

            v1[0] = new_v1x * cos - new_v1y * sin;
            v2[0] = new_v2x * cos - new_v2y * sin;
            v1[1] = new_v1x * sin + new_v1y * cos;
            v2[1] = new_v2x * sin + new_v2y * cos;
        }
    }

    // this function creates a virtual grid in the RAM 
    void gridAllocator() ;

    // this function wipes clean all the data registered in each cell
    void cleanGrid();

    // as we have dynamically allocated memory here , so we need to delete the virtual grid in order prevent memory leak using this function
    void deleteGrid();
};
// this one class stores all the data about particle
class particle :general {
    protected:
        float radius = 0.0f;
        float mass = 0.0f;
        vec3 position = vec3(0.0f,0.0f,0.0f);
        vec3 velocity = vec3(0.0f, 0.0f, 0.0f);
        int divisonX = 0, divisonY = 0;
        int ballNumber = 0;

    public:

        vec3& vel() {
            return velocity;
        }

        float m() {
            return mass;
        }

        float rad() {
            return radius;
        }

        vec3& pos() {
            return position;
        }

        // this function calculate the cell in which the particle's centre belong to 
        void divisionCalculator() {
            // need to be careful that when the position vector is passed it is NDC the converted form is stored in the class!!!

                float shiftedPositionX = position.x + 1.0 ;
                float shiftedPositionY = position.y + 1.0 ;
       
                divisonX = (maxDivison) * shiftedPositionX;

                divisonY = (maxDivison) * shiftedPositionY;// 8 is used in order to decrease the grid size
        }

        // this function fills the individual cells present inside the grid based on the balls position
        void painter() ;

        void dataCollector(int i) {
            std::cout << "mass of object (KG) " << i + 1 << ":";
            std::cin >> particles[i].mass, std::cout << "\n";
            std::cout << "velocity of object " << i + 1 << ":\n";
            std::cout << "X:", std::cin >> particles[i].velocity.x, std::cout << "\n";
            std::cout << "Y:", std::cin >> particles[i].velocity.y, std::cout << "\n";
            std::cout << "Z:", std::cin >> particles[i].velocity.z, std::cout << "\n";
            std::cout << "position of object " << i + 1 << ":\n";
            std::cout << "X:", std::cin >> particles[i].position.x, std::cout << "\n";
            std::cout << "Y:", std::cin >> particles[i].position.y, std::cout << "\n";
            std::cout << "Z:", std::cin >> particles[i].position.z, std::cout << "\n";
            std::cout << "radius of object " << i + 1 << ":\n";
            std::cin >> particles[i].radius;

            particles[i].velocity.x = particles[i].velocity.x / maxVelocity;
            particles[i].velocity.y = particles[i].velocity.y / maxVelocity;
            particles[i].velocity.z = particles[i].velocity.z / maxVelocity;

            particles[i].position.x = particles[i].position.x / maxX;
            particles[i].position.y = particles[i].position.y / maxY;
            particles[i].position.z = particles[i].position.z / maxZ;

            particles[i].radius = particles[i].radius / maxRadius;

            if (smallest_radii > particles[i].radius) {
                smallest_radii = particles[i].radius;
            }

            ballNumber = i;
        }

        // this function defines the containment 
        void changeVelocity() {
            if ((position.x >= 1 - radius || position.x <= -1 + radius )&& (dot(velocity.x,position.x) > 0)) {
                //dot product is being used in order to prevent the particle locking at the edge of the containment
                velocity.x *= (-1);
            }
            if ((position.y >= 1 - radius || position.y <= -1 + radius) && (dot(velocity.y, position.y) > 0)) {
                velocity.y *= (-1);
            }
        }

        void changePosition() {
            position.x += velocity.x*(0.1);
            position.y += velocity.y*(0.1);
            //need to include for z
        }

};

// this is where are the data regarding the grid and cells live
class grid :general {
    private:
    // this is a list dedicated for each cell for knowing how many balls had filled the same cell
        std::list<int> box;
    // this is a list dedicated for the entire grid system in order to prevent uneccesary provoking of collision math for overlapping of cells between the same particles
        static list<int> newBall ;

    public:
    // this function clears the list after drawing each and every ball
        void cleanlist() {
            newBall.clear();
        }
    // fills the cells for a given particles
        void fill(int a , int k , int j) {
        
                if (box.size() == 0) {
                    box.push_back(a);
                }
                
                else {
                    bool is_hitting;
                    for (int i : box) {
                        if(find(newBall.begin(),newBall.end(),i) == newBall.end()){
                            newBall.push_back(i);
                            is_hitting = radiiMath(particles[a].pos(), particles[i].pos(), particles[a].rad(), particles[i].rad());


                            if (is_hitting) {


                                vec3 Prel = particles[a].pos() - particles[i].pos();
              

                                vec3 Vrel = (particles[a].vel()) - (particles[i].vel());
                                //cout << Vrel.x << Vrel.y << Vrel.z << "\n";

                                float Ds = dot(Vrel, Prel);

                                //if (Ds > 0)
                                //cout << Ds << " " << ((particles[a].vel()) - (particles[i].vel())).x << " " << ((particles[a].vel()) - (particles[i].vel())).y << " " << a << " " << i << "<------this is already got hit\n";
                                //else cout << Ds <<" " << ((particles[a].vel()) - (particles[i].vel())).x << " " << ((particles[a].vel()) - (particles[i].vel())).y << " grid no :" << k << " " << j << " " << a << " " << i << "<-------this about to !!  \n";

                                collisionMath(Ds, particles[a].m(), particles[i].m(), value_ptr(particles[a].vel()), value_ptr(particles[i].vel()), particles[a].pos(), particles[i].pos());
                            }
                        }
                    }
                    box.push_back(a);
                }
            
        }

        void clean() {
            box.clear();
        }
};

//early initialisation of static variable
int general::numberOfParticle = 0;
float general::maxVelocity = 0.0f;
float general::maxRadius = 0.0f;
float general::maxX = 0.0f;
float general::maxY = 0.0f;
float general::maxZ = 0.0f;
int general::maxDivison = 0;
float general::smallest_radii = INT_MAX;
grid* general::cell = nullptr;
particle* general::particles = nullptr;
list<int> grid::newBall = {-1};

void general::data(int n, float Vel, float Radius, float X, float Y, float Z) {
    numberOfParticle = n;
    maxVelocity = Vel;
    maxRadius = Radius;
    maxX = X;
    maxY = Y;
    maxZ = Z;
    particles = new particle[n];
}


particle* general::particleArray() {
    return particles;
}

void particle::painter() {
    int scale = (radius*maxDivison);
    
    for (int i = std::max(divisonX - scale,0) ; i <= std::min(divisonX + scale,2*maxDivison - 1); i++) {
        for (int j = std::max(divisonY - scale,0); j <= std::min(divisonY + scale,2*maxDivison - 1); j++) {
            
            (cell + i + 2*j*maxDivison)->fill(ballNumber,i,j);   
        }
    }
    cell->cleanlist();
}

void general::gridAllocator() {
    maxDivison = 8 * (1 / smallest_radii);
    cell = new grid[4*maxDivison * maxDivison];
}

void general::cleanGrid() {
    for (int i = 0; i < 4 * maxDivison * maxDivison; i++) {
        cell[i].clean();
    }
}

void general::deleteGrid() {
    delete[] cell;
}


//function for updating the position of the object 

int main() {
    general Data;
    
	int numberOfObjects;
    unsigned int PBO[2] = {0,0};
    
    float maxVel , maxRD , X , Y , Z;

    cout << "number of objects:", cin >> numberOfObjects, cout << "\n";
    cout << "maximum velocity(m/s):", cin >> maxVel, cout << "\n";
    cout << "maximum radius(m):", cin >> maxRD, cout << "\n";//look at the unit
    cout << "MaxX:", cin >> X, cout << "\n";
    cout << "MaxY:", cin >> Y, cout << "\n";
    cout << "MaxZ:", cin >> Z, cout << "\n";

	Data.data(numberOfObjects,maxVel,maxRD,X,Y,Z);
    

    particle* particles = Data.particleArray();

    for (int i = 0; i < numberOfObjects; i++) {
        particles[i].dataCollector(i);
    }

    Data.gridAllocator();
    

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1200,800,"CollisionSim", NULL, NULL);
	glfwMakeContextCurrent(window);
	functionLoader();
	windowStrecher(window);
	glViewport(0,0,1200,800);

	float vertices[] = {
		0.5f,0.5f,0.0f,
		-0.5f,0.5f,0.0f,
		-0.5f,-0.5f,0.0f,
		0.5f,-0.5f,0.0f
	};

	unsigned int EBO[] = {
		0,1,2,
		2,3,0
	};

    screenRec(1200, 800, 30, "object_collision", PBO);

	bufferCreater(1);
	bufferAttacher(0,0,1);
	bufferData(GL_STATIC_DRAW,vertices,sizeof(vertices),EBO,sizeof(EBO));
	unsigned int VAO = bufferBinder(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	string vertexShader = readShaderFile("vshader.txt");
	string fragmentShader = readShaderFile("fshader.txt");
	const char* vcode = vertexShader.c_str();
	const char* fcode = fragmentShader.c_str();
	const char* shaderCode[] = {vcode , fcode};
	unsigned int shaderProgram = glCompileShaders(2,shaderCode);
	glUseProgram(shaderProgram);

	unsigned int loc0 = glGetUniformLocation(shaderProgram,"mat");

    
    isRecord(1);
	
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
        
		for (int n = 0; n < numberOfObjects; n++) {
            

            particles[n].divisionCalculator();// calculate the division at which the center of the ball is present

            particles[n].painter();//paint that entire spanning square
            
			mat4 matrix = {
				vec4(2*particles[n].rad(), 0.0f,   0.0f,   particles[n].pos().x),
				vec4(0.0f,   2 * particles[n].rad(), 0.0f,   particles[n].pos().y),
				vec4(0.0f,   0.0f,   1.0f,   particles[n].pos().z),
				vec4(0.0f,   0.0f,   0.0f,   1.0f)                
			};

            
            
            glUniformMatrix4fv(loc0, 1, GL_TRUE, value_ptr(matrix));

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            
            particles[n].changePosition();// changes the position

            particles[n].changeVelocity();// changes the velocity if crossin the border
		}

        bufferReader(PBO);
        ifRecord(1, PBO);
        
        Data.cleanGrid();
		screen(window);
		
	}

    delete[]particles ;
    Data.deleteGrid();
    isRecord(0);
	glfwTerminate();
	return 0;
}

