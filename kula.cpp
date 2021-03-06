#include "kula.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>

#include "movablemodel.h"
#include "vertices.h"

Kula::Kula(GLfloat init_velocity, GLfloat init_angle){
    velocity_ = init_velocity;
    angle_ = init_angle;
    animated_ = true;
}


void Kula::Move(GLfloat delta_t){
    if (!animated_) return;
    angle_ += delta_t * velocity_;
    if(angle_>360) angle_ -= 360;
    if(angle_<-360) angle_ += 360;

    // Slonce
    model_matrix_.SetUnitMatrix();
    model_matrix_.RotateAboutY(angle_);

    normal_matrix_.SetUnitMatrix();
    normal_matrix_.RotateAboutY(-angle_);


    //Ziemia
    model_matrix_prim_.SetUnitMatrix();
    model_matrix_prim_.Scale(0.25, 0.25, 0.25);
    model_matrix_prim_.RotateAboutY(3*angle_);
    model_matrix_prim_.Translate(7,0,0);
    model_matrix_prim_.RotateAboutY(angle_);

    normal_matrix_prim_.SetUnitMatrix();
    normal_matrix_prim_.RotateAboutY(-4*angle_);
    normal_matrix_prim_.Scale(4.0,4.0,4.0);

    //Księżyc
    model_matrix_bis_.SetUnitMatrix();
    model_matrix_bis_.Scale(0.1,0.1,0.1);
    model_matrix_bis_.RotateAboutY(3*angle_);
    model_matrix_bis_.Translate(1,0,0);
    model_matrix_bis_.RotateAboutY(3*angle_);
    model_matrix_bis_.Translate(7,0,0);
    model_matrix_bis_.RotateAboutY(angle_);

    normal_matrix_bis_.SetUnitMatrix();
    normal_matrix_bis_.RotateAboutY(-angle_);
    normal_matrix_bis_.RotateAboutY(-3*angle_);
    normal_matrix_bis_.RotateAboutY(-3*angle_);
    normal_matrix_bis_.Scale(50.0,50.0,50.0);
}



void Kula::SpeedUp(){
    velocity_ *= 1.09544511501;
}

void Kula::SlowDown(){
    velocity_ /= 1.09544511501;
}

void Kula::ToggleAnimated(){
    animated_ = ! animated_;
}

void Kula::Initialize(int n, int m, GLfloat R, GLfloat r){
    n_=n;
    m_=m;
    NormalTextureVertex * vertices=new NormalTextureVertex[(m_ + 1)*(n_ + 1)];
    int i,j;
    for (i=0;i<=n_;i++) {
      float phi= ( M_PI/n_ )*i - (M_PI/2);
      for (j=0;j<=m_;j++){
        float theta= (2*M_PI)/(float)m_ *j;
        vertices[i*(m_ + 1)+j].position[0]=(R *cos(phi))*sin(theta);
        vertices[i*(m_ + 1)+j].position[1]=R*sin(phi);
        vertices[i*(m_ + 1)+j].position[2]=(R *cos(phi))*cos(theta);
        vertices[i*(m_ + 1)+j].position[3]=1.0f;
        vertices[i*(m_ + 1)+j].texture[0]=(float)j/(float)m_;
        vertices[i*(m_ + 1)+j].texture[1]=(float)i/(float)n_;
        vertices[i*(m_+1)+j].normal[0]=cos(phi)*sin(theta);
        vertices[i*(m_+1)+j].normal[1]=sin(phi);
        vertices[i*(m_+1)+j].normal[2]=cos(phi)*cos(theta);
      }
    }



    GLuint * indices=new GLuint[2*n_*(m_ + 1)];

    unsigned int k=0;

    for(i=0; i<=n_ - 1; i++){
      for(j=0; j<=m_; j++){
        indices[2*(i*(m_ + 1)+j)]=k;
        indices[2*(i*(m_ + 1)+j)+1]=k+m_+1;
        k++;
      }
    }

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);


    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER,
            (m_+1)*(n_+1)*sizeof(vertices[0]),
            vertices,
            GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
            (GLvoid*)sizeof(vertices[0].position));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
            (GLvoid*)(sizeof(vertices[0].position)+sizeof(vertices[0].texture)));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &index_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (m_+1)*n_*2*sizeof(GLuint),
                 indices,
                 GL_STATIC_DRAW
                 );
    delete [] vertices;
    delete [] indices;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Kula::Draw(const LightProgram &program) const{

    glBindVertexArray(vao_);
    glUseProgram(program);

    glActiveTexture(texture_unit_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    program.SetModelMatrix(model_matrix_);
    program.SetMaterial(material_);
    program.SetNormalMatrix(normal_matrix_);
    for (int i=0;i<n_;i++){
      glDrawElements(GL_TRIANGLE_STRIP,
                     2*(m_ + 1),
                     GL_UNSIGNED_INT,
                     (GLvoid*)(sizeof(GLuint) * 2 * i * (m_ + 1))
                     );
    }

    glActiveTexture(texture_unit_);
    glBindTexture(GL_TEXTURE_2D, texture_prim_);
    program.SetModelMatrix(model_matrix_prim_);
    program.SetMaterial(material_prim_);
    program.SetNormalMatrix(normal_matrix_prim_);
    for (int i=0;i<n_;i++){
      glDrawElements(GL_TRIANGLE_STRIP,
                     2*(m_ + 1),
                     GL_UNSIGNED_INT,
                     (GLvoid*)(sizeof(GLuint) * 2 * i * (m_ + 1))
                     );
    }


    program.SetModelMatrix(model_matrix_bis_);
    program.SetMaterial(material_bis_);
    program.SetNormalMatrix(normal_matrix_bis_);
    glActiveTexture(texture_unit_);
    glBindTexture(GL_TEXTURE_2D, texture_bis_);
    for (int i=0;i<n_;i++){
      glDrawElements(GL_TRIANGLE_STRIP,
                     2*(m_ + 1),
                     GL_UNSIGNED_INT,
                     (GLvoid*)(sizeof(GLuint) * 2 * i * (m_ + 1))
                     );
    }


    glDisable(GL_CULL_FACE);

    glBindVertexArray(0);
    glUseProgram(0);


}
