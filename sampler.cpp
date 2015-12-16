#include "sampler.h"

#include <cstdlib>
#include <cmath>

#include <iostream>
#include <iomanip>
#include <fstream>

void Sampler::generate_samples(Method method)
{
    if (method == RANDOM)
    {
        generate_samples_random();
    }
    else if (method == SOBOL)
    {
        generate_samples_sobol();
    }

    generate_offsets();
}

void Sampler::generate_samples_random()
{
    int nb_total_samples = m_spp * m_dim;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    for (int i = 0; i < nb_total_samples; ++i)
    {
        m_samples.push_back(dis(gen));
    }
}

void Sampler::generate_offsets()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    int nb_offsets = m_width * m_height * m_dim;
    for (int i = 0; i < nb_offsets; ++i)
    {
        m_offsets.push_back(dis(gen));
    }
}


using namespace std;
//from http://web.maths.unsw.edu.au/~fkuo/sobol/
void Sampler::generate_samples_sobol()
{

    int N = m_spp;
    int D = m_dim - 2;
    ifstream infile("../../data/new-joe-kuo-6.21201", ios::in);
    if (!infile) {
        cout << "Input file containing direction numbers cannot be found!\n";
        exit(1);
    }
    char buffer[1000];
    infile.getline(buffer,1000,'\n');

    // L = max number of bits needed
    unsigned L = (unsigned)ceil(log((double)N)/log(2.0));

    // C[i] = index from the right of the first zero bit of i
    unsigned *C = new unsigned [N];
    C[0] = 1;
    for (unsigned i=1;i<=N-1;i++) {
        C[i] = 1;
        unsigned value = i;
        while (value & 1) {
            value >>= 1;
            C[i]++;
        }
    }

    // POINTS[i][j] = the jth component of the ith point
    //                with i indexed from 0 to N-1 and j indexed from 0 to D-1
    double **POINTS = new double * [N];
    for (unsigned i=0;i<=N-1;i++) POINTS[i] = new double [D];
    for (unsigned j=0;j<=D-1;j++) POINTS[0][j] = 0;

    // ----- Compute the first dimension -----

    // Compute direction numbers V[1] to V[L], scaled by pow(2,32)
    unsigned *V = new unsigned [L+1];
    for (unsigned i=1;i<=L;i++) V[i] = 1 << (32-i); // all m's = 1

    // Evalulate X[0] to X[N-1], scaled by pow(2,32)
    unsigned *X = new unsigned [N];
    X[0] = 0;
    for (unsigned i=1;i<=N-1;i++) {
        X[i] = X[i-1] ^ V[C[i-1]];
        POINTS[i][0] = (double)X[i]/pow(2.0,32); // *** the actual points
        //        ^ 0 for first dimension
    }

    // Clean up
    delete [] V;
    delete [] X;


    // ----- Compute the remaining dimensions -----
    for (unsigned j=1;j<=D-1;j++) {

        // Read in parameters from file
        unsigned d, s;
        unsigned a;
        infile >> d >> s >> a;
        unsigned *m = new unsigned [s+1];
        for (unsigned i=1;i<=s;i++) infile >> m[i];

        // Compute direction numbers V[1] to V[L], scaled by pow(2,32)
        unsigned *V = new unsigned [L+1];
        if (L <= s) {
            for (unsigned i=1;i<=L;i++) V[i] = m[i] << (32-i);
        }
        else {
            for (unsigned i=1;i<=s;i++) V[i] = m[i] << (32-i);
            for (unsigned i=s+1;i<=L;i++) {
                V[i] = V[i-s] ^ (V[i-s] >> s);
                for (unsigned k=1;k<=s-1;k++)
                    V[i] ^= (((a >> (s-1-k)) & 1) * V[i-k]);
            }
        }

        // Evalulate X[0] to X[N-1], scaled by pow(2,32)
        unsigned *X = new unsigned [N];
        X[0] = 0;
        for (unsigned i=1;i<=N-1;i++) {
            X[i] = X[i-1] ^ V[C[i-1]];
            POINTS[i][j] = (double)X[i]/pow(2.0,32); // *** the actual points
            //        ^ j for dimension (j+1)
        }

        // Clean up
        delete [] m;
        delete [] V;
        delete [] X;
    }
    delete [] C;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    int nb_total_samples = m_spp * m_dim;
    m_samples.resize(nb_total_samples);
    for (int i = 0; i < m_spp; ++i)
        for (int j = 0; j < m_dim; ++j)
        {
            if (j < 2)
            {
                m_samples[i * m_dim + j] = randf();
            }
            else
            {
                m_samples[i * m_dim + j] = (float) POINTS[i][j - 2];
            }
        }

    delete [] POINTS;
}
