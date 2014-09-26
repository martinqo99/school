/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 217 2012-02-28 17:10:06Z spanel $
 */

#include "student.h"
#include "transform.h"

#include <memory.h>


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renInit(&renderer->base);

    /* nastaveni ukazatelu na vase upravene funkce */
    renderer->base.projectTriangleFunc = studrenProjectTriangle;
    renderer->base.calcReflectanceFunc = studrenPhongReflectance;

    /* ??? */

    return (S_Renderer *)renderer;
}

/****************************************************************************
 * Funkce vyhodnoti Phonguv osvetlovaci model pro zadany bod
 * a vraci barvu, kterou se bude kreslit do frame bufferu
 * point - souradnice bodu v prostoru
 * normal - normala k povrchu v danem bode */

S_RGBA studrenPhongReflectance(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal)
{
    /* ??? */
    S_Coords    lvec;
    double      diffuse, r, g, b;
    S_RGBA      color;
    S_Coords rvec;

    IZG_ASSERT(pRenderer && point && normal);

    /* vektor ke zdroji svetla */
    lvec = makeCoords(pRenderer->light_position.x - point->x,
                      pRenderer->light_position.y - point->y,
                      pRenderer->light_position.z - point->z);
    coordsNormalize(&lvec);

    // diffuse compute
    diffuse = lvec.x * normal->x + lvec.y * normal->y + lvec.z * normal->z;

    // specular part
    S_Coords part = makeCoords(point->x,
                               point->y,
                               point->z - pRenderer->camera_dist);
    coordsNormalize(&part);

    rvec.x = 2.0*diffuse*normal->x - lvec.x;
    rvec.y = 2.0*diffuse*normal->y - lvec.y;
    rvec.z = 2.0*diffuse*normal->z - lvec.z;
    //coordsNormalize(&rvec);

    double result = rvec.x*part.x+ rvec.y*part.y + rvec.z*part.z;
    
    if(result < 0.0)
    {
        r = 0.0; g = 0.0; b = 0.0;
    }
    else
    {
       r = (pRenderer->light_specular.red*pRenderer->mat_specular.blue)*result;
       g = (pRenderer->light_specular.green*pRenderer->mat_specular.blue)*result;
       b = (pRenderer->light_specular.blue*pRenderer->mat_specular.blue)*result;
    }

    /* ambientni cast */
    r += pRenderer->light_ambient.red * pRenderer->mat_ambient.red;
    g += pRenderer->light_ambient.green * pRenderer->mat_ambient.green;
    b += pRenderer->light_ambient.blue * pRenderer->mat_ambient.blue;

    /* difuzni cast */
    if( diffuse > 0 )
    {
        r += diffuse * pRenderer->light_diffuse.red * pRenderer->mat_diffuse.red;
        g += diffuse * pRenderer->light_diffuse.green * pRenderer->mat_diffuse.green;
        b += diffuse * pRenderer->light_diffuse.blue * pRenderer->mat_diffuse.blue;
    }
    
    /* saturace osvetleni*/
    r = MIN(1, r);
    g = MIN(1, g);
    b = MIN(1, b);

    /* kreslici barva */
    color.red = ROUND2BYTE(255 * r);
    color.green = ROUND2BYTE(255 * g);
    color.blue = ROUND2BYTE(255 * b);
    return color;
}

// get minimum value from three points
double getMinimum(double p1, double p2, double p3)
{
    double min = p1;
    if(p2 < min)min = p2;
    if(p3 < min)min = p3;
    return min;
}

// get maximum value from three points
double getMaximum(double p1, double p2, double p3)
{
    double max = p1;
    if(p2 > max)max = p2;
    if(p3 > max)max = p3;
    return max;
}

/****************************************************************************
 * Nova fce pro rasterizace trojuhelniku do frame bufferu
 * vcetne interpolace z souradnice a prace se z-bufferem
 * a interpolaci normaly pro Phonguv osvetlovaci model
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
    S_RGBA color; // color
    double lambda_1 = 0.0; double lambda_2 = 0.0; double lambda_3 = 0.0; // barycentric coordinates
    double z = 0.0; int x = 0; int y = getMinimum(y1,y2,y3);             // x,y,z point
    int max_y = getMaximum(y1,y2,y3); int max_x = getMaximum(x1,x2,x3);  // maximum of x and y in triangle
    S_Coords normalv; S_Coords point;                                    // normal vector and point of pixel

    // repair coordinates of x,y
    if(max_x < 0){max_x = 0;}else if(max_x > pRenderer->frame_w){max_x = pRenderer->frame_w;} 
    if(max_y < 0){max_y = 0;}else if(max_y > pRenderer->frame_h){max_y = pRenderer->frame_h;} 
    if(y < 0){y = 0;}else if(y > pRenderer->frame_h){y = pRenderer->frame_h;}
 
    // go through pixels
    for(y; y < max_y; y++)
    {
        x = getMinimum(x1,x2,x3);
        if(x < 0){x = 0;} else if (x > pRenderer->frame_w){x = pRenderer->frame_w;}
        for(x; x < max_x; x++)
        {
            // compute barycentric coordinates
            lambda_1 = (double)((double)((y2 - y3)*((double)x-x3)) + ((x3 - x2)*((double)y-y3)))/(double)(((y2-y3)*(x1-x3)) + ((x3-x2)*(y1-y3)));
            lambda_2 = (double)((double)((y3 - y1)*((double)x-x3)) + ((x1 - x3)*((double)y-y3)))/((double)((y2-y3)*(x1-x3)) + ((x3-x2)*(y1-y3)));
            lambda_3 = (double)(1.0 - lambda_1 - lambda_2);

            // if point of pixel is in triangle
            if(-0.00001 <= lambda_1 && -0.00001 <= lambda_2 && -0.00001 <= lambda_3 && 1.00001 >= lambda_1 && 1.00001 >= lambda_2 && 1.00001 >= lambda_3)
            {
                // z value of new pixel
                point.z = (lambda_1*v1->z) + (lambda_2*v2->z) + (lambda_3*v3->z);
                z = point.z;

                // if is visible in z-buffer
                if(pRenderer->depth_buffer[(pRenderer->frame_w)*y + x] > z)
                {
                    point.y = (lambda_1*v1->y) + (lambda_2*v2->y) + (lambda_3*v3->y);
                    point.x = (lambda_1*v1->x) + (lambda_2*v2->x) + (lambda_3*v3->x);

                    // interpolation of normal vector in point with barycentric coordinates
                    normalv.x = (n1->x*lambda_1) + (n2->x*lambda_2) + (n3->x*lambda_3);
                    normalv.y = (n1->y*lambda_1) + (n2->y*lambda_2) + (n3->y*lambda_3);
                    normalv.z = (n1->z*lambda_1) + (n2->z*lambda_2) + (n3->z*lambda_3);
                    coordsNormalize(&normalv);

                    // phong lightning
                    color = pRenderer->calcReflectanceFunc(pRenderer, &point, &normalv);

                    // draw the pixel and add z to zbuffer
                    pRenderer->depth_buffer[(pRenderer->frame_w)*y + x] = z;
                    PIXEL(pRenderer, x, y) = color;
                }
            }
        }
    }
}

/*****************************************************************************
 * Vykresli i-ty trojuhelnik modelu
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{
    /* ??? */
    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci ve 3D pred projekci */
    S_Coords    nn,n1,n2,n3;            /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));

    /* z modelu si vytahneme trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* transformace vrcholu matici model */
    trTransformVertex(&aa, cvecGetPtr(pModel->vertices, triangle->v[0]));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, triangle->v[1]));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, triangle->v[2]));

    /* promitneme vrcholy trojuhelniku na obrazovku */
    trProjectVertex(&u1, &v1, &aa);
    trProjectVertex(&u2, &v2, &bb);
    trProjectVertex(&u3, &v3, &cc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, cvecGetPtr(pModel->trinormals, triangle->n));
   
    /* normalizace normaly */
    coordsNormalize(&nn);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }

    // get normal vectors
    trTransformVector(&n1, cvecGetPtr(pModel->normals, triangle->v[0]));
    trTransformVector(&n2, cvecGetPtr(pModel->normals, triangle->v[1]));
    trTransformVector(&n3, cvecGetPtr(pModel->normals, triangle->v[2]));

    // normalization
    coordsNormalize(&n2);
    coordsNormalize(&n3);
    coordsNormalize(&n1);

    studrenDrawTriangle(pRenderer,&aa, &bb, &cc, &n1, &n2, &n3,u1, v1, u2, v2, u3, v3);
}

/*****************************************************************************
 *****************************************************************************/
