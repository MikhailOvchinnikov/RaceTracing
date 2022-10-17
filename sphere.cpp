#include "vector_pict.h"
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>

typedef struct Elem{
	float radius;
	float x;
	float y;
	sf::Color color;
}Elem;

const int SIZE_X = 300;
const float ANGLE_MIN = 0.15;
//int x;
//int y;


float fpow(float x, int degree){
	if(!degree) return (float)1;
	if(degree % 2) return fpow(x, degree - 1);
	float y = fpow(x, degree >> 1);
	return y*y;
}

float ValidAngle(float angle_light){
	if(angle_light < ANGLE_MIN)
		angle_light = ANGLE_MIN;
	return angle_light;
}

float Skal(Vector a, Vector n){
	float result = a.GetX()*n.GetX() + a.GetY()*n.GetY() + a.GetZ()*n.GetZ();
	return result;
}

int CircleTrue(Vector vec, float rad){
	int x_ = vec.GetX();
	int y_ = vec.GetY();
	int res = x_*x_ + y_*y_;
	if(res < rad*rad)
		return 1;
	return 0;
}

int CountZ(int x, int y, float rad){
	return sqrt(rad*rad - x*x - y*y);
}

float CountAngle(Vector a, Vector b){
	float angle_light = Skal(a, b)/(sqrt(a.GetLenPow()*b.GetLenPow()));
	angle_light = ValidAngle(angle_light);
	return angle_light;
}


void CountColor(sf::Color& first, sf::Color& second, float p){
	int r = (first.r + second.r) * p;
	int g = (first.g + second.g) * p;
	int b = (first.b + second.b) * p;
	int a = (first.a + second.a) * p;
	first.r = r;
	first.g = g;
	first.b = b;
	first.a = a;
}


void CountAlpha(sf::Color& first, float alpha){
	first.a *= alpha;
}

void DrawSphere(sf::VertexArray& points, std::vector<Elem>& vec, sf::Color light_col, Vector light_pos){

	sf::Color C_col(255, 255, 255, 255);
	Vector center_to_edge_vec;
	Vector resulted_vec;
	int length = 0;
	float angle_light = 0;
	float angle_camera = 0;
	int size = vec.size();
	for(int i = 0; i < 300;i++)
		for(int j = 0; j < 300;j++){
			int flag = 0;
			int current_point = i*SIZE_X + j;
			points[current_point].position = sf::Vector2f(j, i);
			for(auto it : vec){
				int x_0 = j - it.x;
				int y_0 = i - it.y;
				center_to_edge_vec.SetX(x_0);
				center_to_edge_vec.SetY(y_0);
				if(CircleTrue(center_to_edge_vec, it.radius)){
					center_to_edge_vec.SetZ(CountZ(x_0, y_0, it.radius));
					Vector reduced_vec_to_light = light_pos - center_to_edge_vec;

					length = Skal(reduced_vec_to_light, center_to_edge_vec);
					float n_len = sqrt(center_to_edge_vec.GetLenPow());
					length = (length + n_len)/n_len;
					resulted_vec = center_to_edge_vec*length - center_to_edge_vec;
					resulted_vec = (resulted_vec - reduced_vec_to_light)*2;
					resulted_vec = resulted_vec + reduced_vec_to_light;

					angle_light = CountAngle(reduced_vec_to_light, center_to_edge_vec);
					angle_camera = CountAngle(resulted_vec, light_pos - center_to_edge_vec);
					angle_camera = pow(angle_camera, 15);

					CountColor(it.color, light_col, 0.5);
					CountAlpha(it.color, angle_light);
					CountAlpha(C_col, angle_camera);
					CountColor(it.color, C_col, 0.5);

					points[current_point].color = it.color;
				}else
					flag++;
			}
			if(flag == size)
					points[current_point].color = sf::Color::Black;

		}
}


int main(){
	std::vector<Elem> vec;
	vec.push_back({50, 200, 200, {0, 255, 0, 255}});
	vec.push_back({50, 50, 50, {0, 255, 0, 255}});
	sf::RenderWindow window(sf::VideoMode(300, 300), "Main window");	
	sf::VertexArray points(sf::Points, 300*300);
	Vector light_pos(120, 120, 120);
	sf::Color sphere_col(0, 255, 0 , 255);
	sf::Color light_col(255, 207, 72 , 255);
	bool flag = 0;
	while(window.isOpen()){
		DrawSphere(points, vec, light_col, light_pos);
		sf::Event event;
		while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(points);
		window.display();
	}
	return 0;
}
