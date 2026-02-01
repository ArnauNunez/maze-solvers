//EINES INFORMÀTIQUES PER A LES MATEMÀTIQUES
//AUTOR: Arnau Núñez Martínez
//NIU: 1665983
//DATA: 25/04/2023
//PROGRAMA: escapa.c
//DESCRIPCIÓ: Programa que fa servir diversos algoritmes per a trobar la sortida de qualsevol laberint.

//He fet servir les llibreries wchar.h i locale.h, juntament amb la funció setlocale(LC_ALL, ""), per a que el programa imprimeixi correctament els caràcters especials.
///0. OPCIONS D'EXECUTACIÓ
//Al programa hi ha introduïdes diverses opcions per a si es vol obtenir més informació a l'hora d'excutar el programa. L'opció desitjada s'haurà d'afegir després del nom del laberint.
//Aquestes ocpcions s'expliquen a continuació:

///0.1 -imprimeixlaberint
//Imprimeix el laberint estudiat, abans i després d'efectuar el recorregut. No vàlid per a l'algorisme "borratxo" (només imprimirà el laberint inicial).

///0.2 -casellesexplorades
//Mostra les caselles que s'han visitat

///0.3 -info
//Dona informació variada sobre el recorregut trobat


///1. PREÀMBUL

///1.1 Biblioteques utilitzades
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>

///1.2 Declaració de la cua
// La farem servir per al BFS i l'A*
typedef struct node{
    int x;
    int y;
    int c;
    struct node *seguent;
} cua;

///1.3 Prototipus de les funcions que farem servir

///1.3.1 Inicialització del laberint
int reservalaberint(char *nomlaberint_, char **labnt, int*fils, int*cols);
int crearmatriu(int **A_, const int n, const int m);
int trobar_pos_inicial(int* i, int *j, int *i_0, int *j_0, int files, int columnes, char *laberint);

///1.3.2 Borratxo
int borratxo(int i, int j, const int n, const int m, char *laberint, int files, int columnes, char *opcio, int *ext);

///1.3.3 Backtrack
int backtracking(int *passes, int k, int i, int j,const int n, const int m, int i_0, int j_0, char *laberint, int *A, int files, int columnes, char *opcio, int c, int *ext);

int tractar_solucio(int k, int *passes);

///1.3.4 BFS
int bfs(cua **inici, cua **ultim, int i, int j, const int n, const int m, int i_0, int j_0, char *laberint, int *A, int files, int columnes, char *opcio, int *ext);
int tractar_solucio_bfs(int i, int j, int *anteriors_i, int *anteriors_j, const int m, int i_0, int j_0, char *laberint, int files, int columnes, char *opcio, int ultim_moviment, int p);
int encua(cua **inici, cua **ultim, int x, int y);
void desencua(cua ** inici, int *x, int *y);

///1.3.5 A*
int A_estrella(cua**inici, int i, int j, const int n, const int m, int i_0, int j_0, char *laberint, int *A, int files, int columnes, int i_f, int j_f, char *opcio, int *ext);
int empila(cua ** inici, int x, int y, int c, int i_f, int j_f);
int trobar_sortida(int columnes, int files, int* i_f, int* j_f, char *laberint, int *ext);
int distancia_manhattan(int a_1, int a_2, int b_1, int b_2);
int cost(int i, int j, int i_f, int j_f, int i_0, int j_0);

/// 1.4 Macros
#define laberint(i,j) laberint[columnes * (i) + (j) ]
#define A(i,j) A[m * (i) + (j)]
#define anteriors_i(i,j) anteriors_i[m * (i) + (j)]
#define anteriors_j(i,j) anteriors_j[m * (i) + (j)]

///2. LA FUNCIÓ MAIN
int main(int argc, char* argv[]){
    setlocale(LC_ALL, ""); //Fem que s'imprimeixin bé els caràcters especials
    //Comprovem que els paràmetres introduïts són vàlids
    if ((argc<3)||(argc>4))
    {
        fprintf(stderr, "\nERROR: Paràmetres incorrectes\n\n");
        return 1;
    }
    if ((strcmp(argv[1],"borratxo")!=0)&&(strcmp(argv[1],"backtrack")!=0)&&(strcmp(argv[1],"bfs")!=0)&&(strcmp(argv[1],"A*")!=0))
    {
        fprintf(stderr, "\nERROR: Algorisme no vàlid\n\n");
        return 1;
    }
    if (argc==3)
        argv[3]="null";
    //Inicialitzem el laberint
    int files, columnes, i, j, i_0, j_0;
    char * laberint;
    if((laberint = ( char *) malloc ( sizeof (char) )) == NULL )
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }
    reservalaberint(argv[2], &laberint, &files, &columnes);

    //Obtenim les dimensions del laberint (comptant sols els llocs pels quals ens opdem moure)
    const int n=(files-1)/2;
    const int m=(columnes-1)/2;

    //Trobem la posició (i_0, j_0) en la qual comencem
    trobar_pos_inicial(&i, &j, &i_0, &j_0, files, columnes, laberint);

    if (strcmp(argv[3],"-imprimeixlaberint")==0)
    {
        for(int r=0;r<files;r++)
            {
                for(int s=0;s<columnes;s++)
                    fprintf(stderr, "%c", laberint(r,s));
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n\n");
    int ext=0;
    //Amb això ja podem fer funcionar l'alorisme "borratxo"
    if (strcmp(argv[1],"borratxo")==0)
        borratxo(i, j, n, m, laberint, files, columnes, argv[3], &ext);
    //Inicialitzem la matrius de zeros i uns
    int *A;
    crearmatriu(&A,n,m);
    A(i,j)=1;

    //Fem funcionar l'algorisme "backtrack"
    if (strcmp(argv[1],"backtrack")==0)
    {
        //Inicialitzem el nombre de passes("k") i el vector de moviments
        int *passes;
        if((passes = ( int *) malloc ( sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
        backtracking(passes, 0, i, j, n, m, i_0, j_0, laberint, A, files, columnes, argv[3], 0, &ext);
    }

    //Fem funcionar l'algorisme "BFS"
    else if (strcmp(argv[1],"bfs")==0)
    {
        cua *inici = NULL, *ultim = NULL;
        bfs(&inici, &ultim, i, j, n, m, i_0, j_0, laberint, A, files, columnes, argv[3], &ext);
    }

    //Preparem l'algorisme "A*"
    else if (strcmp(argv[1],"A*")==0)
    {
        //Descobrim on es troba la sortida del laberint
        int i_f, j_f;
        trobar_sortida(columnes, files, &i_f, &j_f, laberint, &ext);
        cua *inici = NULL;
        //Fem funcionar l'algorisme
        A_estrella(&inici, i, j, n, m, i_0, j_0, laberint, A, files, columnes, i_f, j_f, argv[3], &ext);
    }
    //Alliberem l'espai reservat
    free(A);
    free(laberint);
    fflush(stdout);
    fprintf(stderr, "\n\n");
    if (ext==0)
        return 0;
    else
        return 1;
}

///3. INICIALITZACIÓ DEL LABERINT
int reservalaberint(char *nomlaberint_, char **labnt, int*fils, int*cols){

    char nomlaberint;//Declarem les variables que farem servir
    char *laberint, *aux;
    int espais=0, j = 0, i = 0;
    int files=0, columnes=0;
    FILE *obrelaberint;
    char y;
    strcpy(&nomlaberint, nomlaberint_);//Llegim el nom del paràmetre introduït
    puts("\n");
    obrelaberint = fopen(&nomlaberint, "r");//Obrim el laberint en mode lectura
    if (obrelaberint == NULL)//Ens assegurem que el laberint existeix i es pot fer servir
    {
        fprintf(stderr, "\nERROR: El laberint no existeix o no es pot obrir...\n\n");
        exit(1);
    }
    if ((laberint = (char *) malloc ( sizeof (char) ) ) == NULL )
            {
                fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
                exit(1);
            }
    while ((y = getc (obrelaberint)) != EOF)//Llegim el document sencer
    {
        if (y=='\n') //A cada salt de línia, comptem una fila més i ampliem l'espai que ocupa el laberint
        {
            files++;
            if ((aux = (char *) realloc (laberint, (espais+1) * sizeof (char) ) ) == NULL )
            {
                fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
                exit(1);
            }
            laberint=aux;
        }
        else
        espais++;//Cada caràcter que llegim és un espai més que ocuparà el laberint
    }
    columnes=espais/files;//És trivial veure que el nombre de columnes del laberint és el nombre d'espais que hem llegit dividit entre les files comptades
    rewind(obrelaberint);
    while ((y = fgetc (obrelaberint) ) != EOF)//Copiem les dades del fitxer a l'array "laberint"
    {
        if(y == '\n')
        {
            i++;
            j=0;
        }
        else
        {
            laberint(i,j)=y;
            j++;
        }
    }
    fclose(obrelaberint);
    *fils=files;
    *cols=columnes;
    *labnt=laberint;
    return 0;

}

int crearmatriu(int **A_, const int n, const int m){
    int *A;
    A = (int *) malloc (n * m * sizeof (int)); //En reservem l'espai de manera dinàmica amb malloc
    if (A == NULL ) //Comprovem que l'assignació s'ha dut a terme correctament
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }

    for(int a = 0; a < n; a++) //L'omplim de zeros.
    {
        for(int b = 0; b < m; b++)
        {
            A(a,b)=0;
        }
    }
    *A_=A;
    return 0;
}

int trobar_pos_inicial(int* i, int *j, int *i_0, int *j_0, int files, int columnes, char *laberint){
    for(int a = 1; a < files; a++)
    {
        for(int b = 1; b < columnes; b++)
        {
            if(laberint(a,b)=='X') //Trobem la 'X' i guardem els valors de i,j tals que la posició inicial és A[i][j].
            {
                *i=(a-1)/2;
                *j=(b-1)/2;
                *i_0=(a-1)/2;
                *j_0=(b-1)/2;
            }
        }
    }
    return 0;
}

///4. BORRATXO
int borratxo(int i, int j, const int n, const int m, char *laberint, int files, int columnes, char *opcio, int *ext){
    int k=0;//El nombre k compta les passes donades
    srand(time(NULL));//Inicialitzem el generador de nombres pseudoaleatoris
    for( int iteracions=0; iteracions<(n*n*m*m); iteracions++)//Assignem arbitràriament un nombre "raonable" de passes màximes
    {

        if ((i<0)||(j<0)||(i==n)||(j==m)) //Mirem si ja estem fora del laberint
        {
            if (strcmp(opcio,"-casellesexplorades")==0)
            {
                fprintf(stderr, "\n\n");
                for(int r=0;r<files;r++)
                    {
                        for(int s=0;s<columnes;s++)
                            fprintf(stderr, "%c", laberint(r,s));
                        fprintf(stderr, "\n");
                    }
            }

            if (strcmp(opcio,"-info")==0)
            {
                fprintf(stderr, "\n\n INFORMACIÓ:\n\t·PASSES DONADES: %i\n\t·ITERACIONS: %i\n\t·ITERACIONS MÀXIMES: %i", k, iteracions, n*n*m*m);
            }
            return 0;
        }
        if (strcmp(opcio,"-casellesexplorades")==0)
            {
                laberint(2*i+1,2*j+1)='·';
            }
        int u=rand()%4; //En cas negatiu, escollim una direcció a l'atzar
        if ((u==0)&&(laberint(2*i,2*j+1)==' ')) //Mirem si la direcció és acceptable
        {
            k++;
            printf("nord ");//Si ho és, avancem cap a aquella direcció
            i--;
        }
        else if ((u==1)&&(laberint(2*i+2,2*j+1)==' '))
        {
            k++;
            printf("sud ");
            i++;

        }
        else if ((u==2)&&(laberint(2*i+1,2*j+2)==' '))
        {
            k++;
            printf("est ");
            j++;

        }
        else if ((u==3)&&(laberint(2*i+1,2*j)==' '))
        {
            k++;
            printf("oest ");
            j--;

        }
    }
    fprintf(stderr, "\nERROR: El laberint no té sortida...\n\n"); //Si arribem aquí és que hem superat el nombre màxim d'iteracions, pel que el camí no té sortida o no l'hem trobat en un temps "raonable"
    if (strcmp(opcio,"-info")==0)
            {
                fprintf(stderr, "\n\n INFORMACIÓ:\n\t·PASSES DONADES: %i\n\t·ITERACIONS MÀXIMES: %i", k, n*n*m*m);
            }
            return 0;
    *ext=1;
    return 1;
}

///5. BACKTRACKING
int backtracking(int *passes, int k, int i, int j,const int n, const int m, int i_0, int j_0, char *laberint, int *A, int files, int columnes, char* opcio, int c, int *ext){
    int *aux1;
    if ((i<0)||(j<0)||(i==n)||(j==m))//Mirem si ja estem fora del laberint
    {
        tractar_solucio(k, passes);//En cas afirmatiu, tractem la solució
        if (strcmp(opcio,"-info")==0)
            {
                fprintf(stderr, "\n\n INFORMACIÓ:\n\t·PASSES DONADES: %i\n\t·CULS-DE-SAC TROBATS: %i", k, c);
            }

        if ((strcmp(opcio,"-imprimeixlaberint")==0)||(strcmp(opcio,"-casellesexplorades")==0))
        {
            fprintf(stderr, "\n\n");
            for(int r=0;r<files;r++)
            {
                for(int s=0;s<columnes;s++)
                    fprintf(stderr, "%c", laberint(r,s));
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
        }
        return 0;
    }
    else if (((laberint(2*i,2*j+1)==' ')&&(A(i-1,j)==0))||((i==0)&&(laberint(2*i,2*j+1)==' '))) //En cas negatiu, avancem cap a la primera direcció on es pugui
    {

        k++; //Comptem una passa més
        if((aux1 = ( int *) realloc ( passes, (k+1) * sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
        passes=aux1;
        passes[k]=0; //Ens guardem la direcció que hem pres
        A(i,j)=1; //Marquem la casella com a visitada
        if (strcmp(opcio,"-imprimeixlaberint")==0)
        {
            laberint(2*i+1,2*j+1)='^';
            laberint(2*i,2*j+1)='^';
        }
        if (strcmp(opcio,"-casellesexplorades")==0)
            {
                laberint(2*i+1,2*j+1)='·';
            }
        i--; //Avancem cap a la direcció corresponent
        backtracking(passes, k, i, j, n, m, i_0, j_0, laberint, A, files, columnes, opcio, c, ext);//Repetim el procés
        return 0;
    }

    else if (((laberint(2*i+2,2*j+1)==' ')&&(A(i+1,j)==0))||((i==n-1)&&(laberint(2*i+2,2*j+1)==' ')))
    {
        k++;
        if((aux1 = ( int *) realloc ( passes, (k+1) * sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
        passes=aux1;
        passes[k]=1;
        A(i,j)=1;
        if (strcmp(opcio,"-imprimeixlaberint")==0)
        {
            laberint(2*i+1,2*j+1)='v';
            laberint(2*i+2,2*j+1)='v';
        }
        if (strcmp(opcio,"-casellesexplorades")==0)
            {
                laberint(2*i+1,2*j+1)='·';
            }
        i++;
        backtracking(passes, k, i, j, n, m, i_0, j_0, laberint, A, files, columnes, opcio, c, ext);
        return 0;
    }
    else if (((laberint(2*i+1,2*j+2)==' ')&&(A(i,j+1)==0))||((j==m-1)&&(laberint(2*i+1,2*j+2)==' ')))
    {
        k++;
        if((aux1 = ( int *) realloc ( passes, (k+1) * sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
        passes=aux1;
        passes[k]=2;
        A(i,j)=1;
        if (strcmp(opcio,"-imprimeixlaberint")==0)
        {
            laberint(2*i+1,2*j+1)='>';
            laberint(2*i+1,2*j+2)='>';
        }
        if (strcmp(opcio,"-casellesexplorades")==0)
            {
                laberint(2*i+1,2*j+1)='·';
            }
        j++;
        backtracking(passes, k, i, j, n, m, i_0, j_0, laberint, A, files, columnes, opcio, c, ext);
        return 0;
    }
    else if (((laberint(2*i+1,2*j)==' ')&&(A(i,j-1)==0))||((j==0)&&(laberint(2*i+1,2*j)==' ')))
    {
        k++;
        if((aux1 = ( int *) realloc ( passes, (k+1) * sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
        passes=aux1;
        passes[k]=3;
        A(i,j)=1;
        if (strcmp(opcio,"-imprimeixlaberint")==0)
        {
            laberint(2*i+1,2*j+1)='<';
            laberint(2*i+1,2*j)='<';
        }
        if (strcmp(opcio,"-casellesexplorades")==0)
            {
                laberint(2*i+1,2*j+1)='·';
            }
        j--;
        backtracking(passes, k, i, j, n, m, i_0, j_0, laberint, A, files, columnes, opcio, c, ext);
        return 0;
    }
    else if((i==i_0)&&(j==j_0)) //Si estem a l'inici i no ens podem moure, el laberint no té solució
    {
        fprintf(stderr, "\nERROR: El laberint no té sortida...\n\n");
        free(passes);
        *ext=1;
        return 1;
    }
    else //Si arribem a un cul-de-sac...
    {
        A(i,j)=1; //Marquem la casella com a visitada
        c++;
        if (passes[k]==0) //Desfem l'ultima passa donada
            {
                if (strcmp(opcio,"-imprimeixlaberint")==0)
                {
                    laberint(2*i+1,2*j+1)=' ';
                    laberint(2*i+2,2*j+1)=' ';
                }
                i++;
            }
        else if (passes[k]==1)
            {
                if (strcmp(opcio,"-imprimeixlaberint")==0)
                {
                    laberint(2*i+1,2*j+1)=' ';
                    laberint(2*i,2*j+1)=' ';
                }
                i--;
            }
        else if (passes[k]==2)
            {
                if (strcmp(opcio,"-imprimeixlaberint")==0)
                {
                    laberint(2*i+1,2*j+1)=' ';
                    laberint(2*i+1,2*j)=' ';
                }
                j--;
            }
        else if (passes[k]==3)
            {
                if (strcmp(opcio,"-imprimeixlaberint")==0)
                {
                    laberint(2*i+1,2*j+1)=' ';
                    laberint(2*i+1,2*j+2)=' ';
                }
                j++;
            }
        k--;
        if((aux1 = ( int *) realloc ( passes, (k+1) * sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
        passes=aux1;
        backtracking(passes, k, i, j, n, m, i_0, j_0, laberint, A, files, columnes, opcio, c, ext); //Repetim el procés
        return 0;
    }
    return 1;
}

int tractar_solucio(int k, int*passes){
    for(int w=1; w<=k; w++) //Li fem correspondre a cada nombre (0, 1, 2, 3) la direcció que li pertoqui(nord, sud, est, oest)
    {
        if (passes[w]==0)
            printf("nord ");
        else if (passes[w]==1)
            printf("sud ");
        else if (passes[w]==2)
            printf("est ");
        else if (passes[w]==3)
            printf("oest ");
    }
    return 0;
}

///6. BFS
int bfs(cua **inici, cua **ultim, int i, int j, const int n, const int m, int i_0, int j_0, char *laberint, int *A, int files, int columnes, char *opcio, int *ext){
    int*anteriors_i, *anteriors_j; //Inicialitzem les matrius "anteriors", una per a cada component
    int k=0;
    anteriors_i = (int *) malloc (n * m * sizeof (int)); //En reservem l'espai de manera dinàmica amb malloc.
    if (anteriors_i == NULL ) //Comprovem que l'assignació s'ha dut a terme correctament.
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }

    anteriors_j = (int *) malloc (n * m * sizeof (int)); //En reservem l'espai de manera dinàmica amb malloc.
    if (anteriors_j == NULL ) //Comprovem que l'assignació s'ha dut a terme correctament.
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }
    for(int a = 0; a < n; a++) //Les omplim de zeros.
    {
        for(int b = 0; b < m; b++)
        {
            anteriors_j(a,b)=0;
            anteriors_i(a,b)=0;
        }
    }

    encua(inici, ultim, i, j); //Afegim la primera posició a la cua
    while (*inici!=NULL)//Repetim el procés mentre la cua no sigui buida, és a dir, mentre hi hagi candidats
    {
        desencua(inici, &i, &j); //Traiem i i j de la primera posició que hem afegit a la llista
        k++;
        if ((i<0)||(j<0)||(i==n)||(j==m)) //Mirem si hem sortit ja del laberint
        {
            tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 0, k); //En cas afirmatiu, tractem la solució
            return 0;
        }
        if (strcmp(opcio,"-casellesexplorades")==0)
            {
                laberint(2*i+1,2*j+1)='·';
            }
        //En cas negatiu, examinem cada candidat. Un candidat és acceptable si es pot anar cap a la direcció corresponent (i no hi hem estat abans) o si surt del laberint
        if (((laberint(2*i,2*j+1)==' ')&&(A(i-1,j)==0))||((i==0)&&(laberint(2*i,2*j+1)==' ')))
        {
            if ((anteriors_i(i-1, j)!=0)||(anteriors_j(i-1, j)!=0))//En cas que surti del laberint, com que estem treballant amb matrius pensades com a vectors n x m, a la posició de la "sortida" se li assignaran les
            {//coordenades del primer element de la següent fila ocolumna (o, fins i tot, nombres aleatoris!). Si la següent posició correspon a valors (0,0) a les matrius anteriors, la funció pot reescriure-hi com si es
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 1, k);//tractés d'una posició "extra" a la fila o columna corresponent. Si no, tractarem la posició anterior a la sortida com a solució i després
                return 0;//hi afegirem la direcció a la que es troba la sortida, de manera que el resultat serà equivalent.
            }
            encua(inici, ultim, i-1, j);//Afegim a la cua el candidat explorat
            A(i-1,j)=1; //Marquem la casella com a visitada
            anteriors_i(i-1, j)=i; //Ens guardem la posició a les matrius anteriors, de manera que a la seguent posició li correspondran els valors (i,j) de la casella actual. D'aquesta manera, cada posició del laberint
            anteriors_j(i-1, j)=j; //"té" a les matrius anteriors informació sobre quina és la posició anterior, de manera que cada casella "apunta" a la casella anterior, fins aarribar a la posició incial.
        }
        if (((laberint(2*i+2,2*j+1)==' ')&&(A(i+1,j)==0))||((i==n-1)&&(laberint(2*i+2,2*j+1)==' ')))
        {
            if ((anteriors_i(i+1, j)!=0)||(anteriors_j(i+1, j)!=0))
            {
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 2, k);
                return 0;
            }
            encua(inici, ultim, i+1, j);
            A(i+1,j)=1;
            anteriors_i(i+1, j)=i;
            anteriors_j(i+1, j)=j;
        }
        if (((laberint(2*i+1,2*j+2)==' ')&&(A(i,j+1)==0))||((j==m-1)&&(laberint(2*i+1,2*j+2)==' ')))
        {
            if ((anteriors_i(i, j+1)!=0)||(anteriors_j(i, j+1)!=0))
            {
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 3, k);
                return 0;
            }
            encua(inici, ultim, i, j+1);
            A(i,j+1)=1;
            anteriors_i(i, j+1)=i;
            anteriors_j(i, j+1)=j;
        }
        if (((laberint(2*i+1,2*j)==' ')&&(A(i,j-1)==0))||((j==0)&&(laberint(2*i+1,2*j)==' ')))
        {
            if ((anteriors_i(i, j-1)!=0)||(anteriors_j(i, j-1)!=0))
            {
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 4, k);
                return 0;
            }
            encua(inici, ultim, i, j-1);
            A(i,j-1)=1;
            anteriors_i(i, j-1)=i;
            anteriors_j(i, j-1)=j;
        }
    }
    fprintf(stderr, "\nERROR: El laberint no té sortida...\n\n");
    *ext=1;
    return 1;
}

///6.1 Funció tractar_solució (de forma no recursiva)
int tractar_solucio_bfs(int i, int j, int *anteriors_i, int *anteriors_j, const int m, int i_0, int j_0, char *laberint, int files, int columnes, char*opcio, int ultim_moviment, int p){
    int a=i;
    int b=j;
    int t=0;
    int *solucio, *aux2; //El vector solució contindrà les passes que es donen per arribar pel camí més curt de la casella d'arribada a la casella d'inici
    int k=0;
    solucio = (int *) malloc (k*sizeof (int));
    if (solucio == NULL )
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }
    //Tal i com està esmentat anteriorment, cada casella "apunta" a l'anterior mitjançant les matrius anteriors. D'aquesta manera, la "casella solució", que és la que s'ha introduit en aquesta funció, es pot rastrejar
    //per trobar-ne el camí més ràpid que la porta a la casella d'inici
    while ((a!=i_0)||(b!=j_0)) //El procès s'aturarà quan arribem a la casella inicial
    {
        if((aux2 = ( int *) realloc ( solucio, (k+2) * sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
        solucio=aux2;
        solucio[k]=a;
        solucio[k+1]=b;//Afegim al vector la posició de la casella en la que ens trobem
        k=k+2;
        int c=anteriors_i(a,b); //Ens movem a la casella anterior
        int d=anteriors_j(a,b);
        a = c;
        b = d;
        t++;
    }
    if((aux2 = ( int *) realloc ( solucio, (k+2) * sizeof ( int) )) == NULL )
        {
            fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
            exit(1);
        }
    solucio=aux2;
    solucio[k]=a; //Afegim la casella d'inici al vector
    solucio[k+1]=b;
    for(int u=k+1; u>2; u-=2) //Un cop ja tenim el vector, l'hem d'analitzar en ordre invertit per a veure quines direccions hem pres per arribar de l'inici a la sortida del laberint.
    {
        if (solucio[u-1]>solucio[u-3]){
            printf("nord ");
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*solucio[u-1]+1,2*solucio[u]+1)='^';
                laberint(2*solucio[u-1],2*solucio[u]+1)='^';
            }
        }
        if (solucio[u-1]<solucio[u-3]){
            printf("sud ");
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*solucio[u-1]+1,2*solucio[u]+1)='v';
                laberint(2*solucio[u-1]+2,2*solucio[u]+1)='v';
            }
        }
        if (solucio[u]<solucio[u-2]){
            printf("est ");
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*solucio[u-1]+1,2*solucio[u]+1)='>';
                laberint(2*solucio[u-1]+1,2*solucio[u]+2)='>';
            }
        }
        if (solucio[u]>solucio[u-2]){
            printf("oest ");
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*solucio[u-1]+1,2*solucio[u]+1)='<';
                laberint(2*solucio[u-1]+1,2*solucio[u])='<';
            }
        }
    }
    if (ultim_moviment==1){
            printf("nord ");
            t++;
            p++;
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*i+1,2*j+1)='^';
                laberint(2*i,2*j+1)='^';
            }
        }
        if (ultim_moviment==2){
            printf("sud ");
            t++;
            p++;
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*i+1,2*j+1)='v';
                laberint(2*i+2,2*j+1)='v';
            }
        }
        if (ultim_moviment==3){
            printf("est ");
            t++;
            p++;
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*i+1,2*j+1)='>';
                laberint(2*i+1,2*j+2)='>';
            }
        }
        if (ultim_moviment==4){
            printf("oest ");
            t++;
            p++;
            if (strcmp(opcio,"-imprimeixlaberint")==0)
            {
                laberint(2*i+1,2*j+1)='<';
                laberint(2*i+1,2*j)='<';
            }
        }

    if ((strcmp(opcio,"-imprimeixlaberint")==0)||(strcmp(opcio,"-casellesexplorades")==0))
        {
            fprintf(stderr, "\n\n");
            for(int r=0;r<files;r++)
            {
                for(int s=0;s<columnes;s++)
                    fprintf(stderr, "%c", laberint(r,s));
                fprintf(stderr, "\n");
            }
        }
    if (strcmp(opcio,"-info")==0)
        {
            fprintf(stderr, "\n\n INFORMACIÓ:\n\t·CASELLES EXPLORADES: %i\n\t·LONGITUD DEL CAMÍ: %i", p-1, t);
        }
    return 0;
}

///6.2 La cua
//Per a la funció bfs, sols fem servir les components x i y de cada node de la cua
int encua(cua ** inici, cua **ultim, int x, int y){
    cua *nou = NULL; //reservem l'espai per al nou node
    if((nou = (cua*) malloc(sizeof(cua)))==NULL)
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }
    nou->x = x;
    nou->y = y;

    if (*inici==NULL)//Si la cua és buida, afegim a l'inici
    {
        nou->seguent = *inici;
        *inici = nou;
        *ultim = *inici;
    }
    else //Si no, afegim al final
    {
        nou->seguent=NULL;
        (*ultim)->seguent = nou;
        *ultim = nou;
    }
    return 0;
}

void desencua(cua ** inici, int *x, int *y){
    cua *aux = NULL;
    if((aux = (cua*) malloc(sizeof(cua)))==NULL)
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }
    *x=(*inici)->x; //Guardem els valors de les variables del node estudiat a x i y per a poder fer servir-les posteriorment a la funció bfs
    *y=(*inici)->y;
    aux = *inici;
    *inici = (*inici)->seguent;
    free(aux); //Eliminem el node
}

///7. A*
//L'elaboració d'aquest algorisme s'ha pensat com a evolució de l'algorisme BFS. El principal canvi és, que, enlloc d'explorar els diferents
//candidats segons l'ordre pel que s'han visitat anteriorment, es visita sols el candidat que té el cost més baix, de manera que es fa servir
//l'heurística per decidir cap a on ens mourem en cada cas.

//Per a fer-ho possible, s'ha repensat la cua feta servir a l'algorisme del BFS com a llista enllaçada i ordenada segons el cost (c) de cada
//candidat, de petit a gran. D'aquesta manera, cada cop que s'explora un node s'afegeix el candidat corresponent al lloc de la llista que li
//pertoca segons el valor del seu cost. Es reutilitza la funció desencuar, ja que sols es treuran nodes del principi, de manera que sols
//s'explorarà en cada cas el candidat de menys cost.
int A_estrella(cua**inici, int i, int j, const int n, const int m, int i_0, int j_0, char *laberint, int *A, int files, int columnes, int i_f, int j_f, char *opcio, int *ext){
    int*anteriors_i, *anteriors_j; //Inicialitzem les matrius "anteriors", una per a cada component
    int p=0;
    anteriors_i = (int *) malloc (n * m * sizeof (int)); //En reservem l'espai de manera dinàmica amb malloc.
    if (anteriors_i == NULL ) //Comprovem que l'assignació s'ha dut a terme correctament.
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }

    anteriors_j = (int *) malloc (n * m * sizeof (int)); //En reservem l'espai de manera dinàmica amb malloc.
    if (anteriors_j == NULL ) //Comprovem que l'assignació s'ha dut a terme correctament.
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }
    for(int a = 0; a < n; a++) //L'omplim de zeros.
    {
        for(int b = 0; b < m; b++)
        {
            anteriors_j(a,b)=0;
            anteriors_i(a,b)=0;
        }
    }
    int c=cost(i, j, i_f, j_f, i_0, j_0); //Calculem el cost del primer candidat
    empila(inici, i, j, c, i_f, j_f); //Afegim la primera posició a la cua
    while ((*inici)!=NULL)//Repetim el procés mentre la cua no sigui buida, és a dir, mentre hi hagi candidats
    {
        desencua(inici, &i, &j); //Traiem i i j de la primera posició de la llista
        p++;

        if ((i<0)||(j<0)||(i==n)||(j==m)) //Mirem si hem sortit ja del laberint
        {
            tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 0, p); //En cas afirmatiu, tractem la solució
            return 0;
        }
        if (strcmp(opcio,"-casellesexplorades")==0)
            {
                laberint(2*i+1,2*j+1)='·';
            }
        //En cas negatiu, examinem cada candidat. Un candidat és acceptable si es pot anar cap a la direcció corresponent (i no hi hem estat abans) o si surt del laberint
        if (((laberint(2*i,2*j+1)==' ')&&(A(i-1,j)==0))||((i==0)&&(laberint(2*i,2*j+1)==' ')))
        {
            if ((anteriors_i(i-1, j)!=0)||(anteriors_j(i-1, j)!=0))//En cas que surti del laberint, com que estem treballant amb matrius pensades com a vectors n x m, a la posició de la "sortida" se li assignaran les
            {//coordenades del primer element de la següent fila ocolumna (o, fins i tot, nombres aleatoris!). Si la següent posició correspon a valors (0,0) a les matrius anteriors, la funció pot reescriure-hi com si es
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 1, p);//tractés d'una posició "extra" a la fila o columna corresponent. Si no, tractarem la posició anterior a la sortida com a solució i després
                return 0;//hi afegirem la direcció a la que es troba la sortida, de manera que el resultat serà equivalent.
            }
            c=cost(i-1, j, i_f, j_f, i_0, j_0);//Calculem el cost del  candidat
            empila(inici, i-1, j, c, i_f, j_f);//Afegim el candidat a la cua segons el seu cost
            A(i-1,j)=1; //Marquem la casella com a visitada
            anteriors_i(i-1, j)=i; //Ens guardem la posició a les matrius anteriors, de manera que a la seguent posició li correspondran els valors (i,j) de la casella actual. D'aquesta manera, cada posició del laberint
            anteriors_j(i-1, j)=j; //"té" a les matrius anteriors informació sobre quina és la posició anterior, de manera que cada casella "apunta" a la casella anterior, fins aarribar a la posició incial.
        }
        if (((laberint(2*i+2,2*j+1)==' ')&&(A(i+1,j)==0))||((i==n-1)&&(laberint(2*i+2,2*j+1)==' ')))
        {
            if ((anteriors_i(i+1, j)!=0)||(anteriors_j(i+1, j)!=0))
            {
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 2, p);
                return 0;
            }
            c=cost(i+1, j, i_f, j_f, i_0, j_0);
            empila(inici, i+1, j, c, i_f, j_f);
            A(i+1,j)=1;
            anteriors_i(i+1, j)=i;
            anteriors_j(i+1, j)=j;
        }
        if (((laberint(2*i+1,2*j+2)==' ')&&(A(i,j+1)==0))||((j==m-1)&&(laberint(2*i+1,2*j+2)==' ')))
        {
            if ((anteriors_i(i, j+1)!=0)||(anteriors_j(i, j+1)!=0))
            {
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 3, p);
                return 0;
            }
            c=cost(i, j+1, i_f, j_f, i_0, j_0);
            empila(inici, i, j+1, c, i_f, j_f);
            A(i,j+1)=1;
            anteriors_i(i, j+1)=i;
            anteriors_j(i, j+1)=j;
        }
        if (((laberint(2*i+1,2*j)==' ')&&(A(i,j-1)==0))||((j==0)&&(laberint(2*i+1,2*j)==' ')))
        {
            if ((anteriors_i(i, j-1)!=0)||(anteriors_j(i, j-1)!=0))
            {
                tractar_solucio_bfs(i,j, anteriors_i, anteriors_j, m, i_0, j_0, laberint, files, columnes, opcio, 4, p);
                return 0;
            }
            c=cost(i, j-1, i_f, j_f, i_0, j_0);
            empila(inici, i, j-1, c, i_f, j_f);
            A(i,j-1)=1;
            anteriors_i(i, j-1)=i;
            anteriors_j(i, j-1)=j;
        }
    }
    fprintf(stderr,"ERROR: El laberint no té sortida...");
    *ext=1;
    return 1;
}

///7.1 La llista
//Per a la funció A_estrella, fem servir també la component c, que indica el cost de moure's a cada posició
int empila(cua ** inici, int x, int y, int c, int i_f, int j_f){

    cua *nou = NULL, *actual = NULL;
    if((nou = (cua*) malloc(sizeof(cua)))==NULL)//Reservem l'espai per al nou node
    {
        fprintf(stderr, "\nERROR: No és possible assignar la memòria necessària...\n\n");
        exit(1);
    }
    nou->x = x;
    nou->y = y;
    nou->c = c;
//    int k=1;
    if ((*inici==NULL)||((*inici)->c>c))//Si la llista és buida o el candidat explorat té un cost menor que tota la resta, introduïm a l'inici
    {
        nou->seguent = *inici;
        *inici = nou;
    }
    else
    {
       actual = *inici; //Si no, busquem la posició que li correspon per ordre creixent de cost. En cas d'empat, anirà abans el node que estigui a menys distància de la sortida
//       k++;

       while ((actual->seguent != NULL)&& ((actual->seguent->c<c)||((actual->seguent->c==c)&&((distancia_manhattan(actual->x, actual->y, i_f, j_f))>(distancia_manhattan(actual->seguent->x, actual->seguent->y, i_f, j_f))))))
            {actual = actual->seguent;
//            k++;
            }
        nou->seguent = actual->seguent;
      actual->seguent = nou;
    }
//    printf("He posat el node (%i, %i) a la posició %i\n", x, y, k);
    return 0;
}

///7.2 Trobar la sortida del laberint
int trobar_sortida(int columnes, int files, int* i_f, int* j_f, char *laberint, int *ext){

    int a=0;
        for(int b = 1; b < columnes; b+=2)
        {
            if(laberint(a,b)==' ')  //Trobem la sortida i guardem els valors de i,j tals que la posició final és A[i][j].
            {                       //Notem que la posició indicada està "fora" del laberint; és una posició "fictícia". No existeix realment, però aquests valors de i i j ens serveixen  per a calcular la distància manhattan a
                *i_f=-1;            //la sortida amb precisió, i també per a fer anar amb normalitat la funció tractar_solucio_bfs
                *j_f=(b-1)/2;
                return 0;
            }
        }
    a=files-1;
    for(int b = 1; b < columnes; b+=2)
        {
            if(laberint(a,b)==' ')
            {
                *i_f=(files-1)/2;
                *j_f=(b-1)/2;
                return 0;
            }
        }
    int b=0;
        for(int a = 1; a < files; a+=2)
        {
            if(laberint(a,b)==' ')
            {
                *i_f=(a-1)/2;
                *j_f=-1;
                return 0;
            }
        }
    b=columnes-1;
        for(int a = 1; a < files; a+=2)
        {
            if(laberint(a,b)==' ')
            {
                *i_f=(a-1)/2;
                *j_f=(columnes-1)/2;
                return 0;

            }
        }
    fprintf (stderr, "\nERROR: El laberint no té sortida...\n\n"); //Si no es troba cap sortida, és que no n'hi ha pas.
    *ext=1;
    return 1;
}

///7.3 "Cost" dels nodes. La funció heurística
int distancia_manhattan(int a_1, int a_2, int b_1, int b_2){
    //Calcula la distància manhattan entre dos punts A i B qualssevol, seguint la fòrmula següent, vàlida per a dues dimensions:
    //distància_manhattan(A,B) = | a_1 - b_1 | + | a_2 - b_2 |
    int d, d_1, d_2;
    if ((d_1=a_1-b_1)<0)
        d_1=(-d_1);
    if ((d_2=a_2-b_2)<0)
        d_2=(-d_2);
    d=d_1+d_2;
    return d;

}

int cost(int i, int j, int i_f, int j_f, int i_0, int j_0){
    //El "cost" d'un candidat n en un algorisme es calcula com f(n) = g(n) + h(n), on g(n) és el cost de moure's des del primer node fins a n, i h(n) una funció heurística qu estima el cost de la ruta més curta
    //fins a la sortida. En aquest cas, el cost és la ditància manhattan entre el candidat i la posició inicial més la distància manhattan entre el candidat i la sortida.
    int f = distancia_manhattan(i_0, j_0, i, j) + distancia_manhattan(i, j, i_f, j_f);
    return f;
}

