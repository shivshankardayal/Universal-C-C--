
typedef struct matrix {
    double index[4][4];
} Matrix;

typedef struct vector {
    double index[4];
} Vector;

void init_matrix(Matrix *m);
void init_vector(Vector *v);

#define copy_matrix(m1,m2)	memcpy(m1,m2,sizeof(*m1))
#define copy_vector(v1,v2) memcpy(v1,v2,sizeof(*v1))

void multiply_matrix(Matrix *m1, Matrix *m2);
void multiply_vector_matrix(Matrix *m, Vector *v);
void multiply_matrix_vector(Matrix *m, Vector *v);
void rotate_x(double angle, Matrix *m);
void rotate_y(double angle, Matrix *m);
void rotate_z(double angle, Matrix *m);
