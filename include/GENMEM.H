#include <classlib\arrays.h>

/***********************************************************************
Class Declarations
***********************************************************************/


class genmember
{
	public:
	 genmember();
	 int membersize;        // length of each genome
	 int minval, maxval;    // range of cell values
	 int bits[100];         // representation of genome
	 void mutate();
	 void create(int, int, int);
	 int fitness;           // holds the fitness rating as determined externally
	 operator ==(genmember);
};

class generation
{
	public:
	 generation(int GENSIZE, int MEMBERSIZE, int MINVAL, int MAXVAL);
	 ~generation();

	 TArrayAsVector<genmember> member;  // array of genomes
	 int gensize;                       // size of population
	 int membersize;                    // as in 'genmember' class
	 int minval, maxval;                // as in 'genmember' class
	 long generationnumber;
	 TArrayAsVector<long> previousfitness;

/***********************************************************************
Function declarations.  Functions are described in file GENMEM.CPP
***********************************************************************/
	 void create();
	 void reproduce();
	 int getbestmember();
	 void mate(int mem1id, int mem2id, genmember& newmember);
	 int selectmember();
	 long gettotalfitness();
	private:
	 int newmemberduplicate(int);

	 TArrayAsVector<genmember> newmember; // temporary array used when creating
};                                       // new generation