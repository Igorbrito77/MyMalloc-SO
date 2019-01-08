#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define foreach(ptr, lista) for(ptr = *lista; ptr != NULL; ptr = ptr->prox)
#define BackHeader(ptr) (((Bloco)ptr)-1);
#define ALIGN8(size)  


typedef struct Bloco
{
	struct Bloco* prox;
	struct Bloco* ant;
	
	union {
	
			struct {
				unsigned tamanho: 31;
				unsigned livre: 1;
			}Flag;

	}Registrador;
}*Bloco;


struct 
{
 	void ** base_dados;
 	Bloco ultimo_acessado;
 	Bloco* blocos_livres;	
}Gerencia; 


static unsigned long memoria_utilizada = 0;

void __attribute__((constructor)) InitMalloc(){

	void* requisitada = NULL;

	Gerencia.base_dados = sbrk(0); // Seta o endereço da base de dados
	requisitada = sbrk(sizeof(void**));
	assert(!(requisitada == (void*)-1));

	memoria_utilizada += sizeof(void**);

	*(Gerencia.base_dados) = NULL; //Base de dados ainda nao referenciada
	assert(!(requisitada == (void*)-1));

	Gerencia.ultimo_acessado = NULL;

	Gerencia.blocos_livres = sbrk(0); // Seta o endereço dos blocos livres
	requisitada = sbrk(sizeof(Bloco*));
	assert(!(requisitada == (void*)-1));

	memoria_utilizada += sizeof(Bloco*);
	*(Gerencia.blocos_livres) = NULL; // Blocos de livres ainda nao referenciados
}

void* MyMalloc(unsigned int tamanho){

	Bloco novo_bloco;
	Bloco pesquisa;
	void* temp;

	if(tamanho <= 0)
		return NULL;

	if( (*(Gerencia.base_dados)) == NULL){ // Base ainda não referenciada

		novo_bloco = sbrk(0); //endereço
		temp = sbrk(tamanho + sizeof(struct Bloco));
		if(temp == (void*)-1) // se não houver memoria disponivel
			return NULL;

		*(Gerencia.base_dados) = novo_bloco;
		novo_bloco->prox = NULL;
		novo_bloco->ant = NULL;
		novo_bloco->Registrador.Flag.tamanho = tamanho;
		novo_bloco->Registrador.Flag.livre = 0;
	}

	else{
		foreach(pesquisa, (Gerencia.base_dados)){
			if(pesquisa->Registrador.Flag.tamanho >= tamanho && pesquisa->Registrador.Flag.livre){
				pesquisa->Registrador.Flag.tamanho = tamanho;
				pesquisa->Registrador.Flag.livre = 0;
				return pesquisa;
			}
		}

		novo_bloco = sbrk(0);
		temp = sbrk(tamanho + sizeof(struct Bloco));
		if(temp == (void*)-1)
			return NULL;

		novo_bloco->ant = NULL;
		novo_bloco->Registrador.Flag.tamanho = tamanho;

		novo_bloco->prox  = ((Bloco) (*(Gerencia.base_dados)) );
		((Bloco) (*(Gerencia.base_dados)) )->ant = novo_bloco;
		(*(Gerencia.base_dados)) = novo_bloco; 

	}

	Bloco aux = *(Gerencia.base_dados);
	printf("ponta = %d\n", aux->Registrador.Flag.tamanho);

	return (novo_bloco+1); // retorna o endereço do novo bloco

}


int MyMallocFree(void* ptr){

	Bloco bloco_livre;
	Bloco aux;
	
	assert((long long) (ptr));
	bloco_livre = BackHeader(ptr);
	bloco_livre->Registrador.Flag.livre = 1;

	aux = (*(Gerencia.base_dados));
//	printf("ponta = %d\n", aux->Registrador.Flag.tamanho);
//	printf("bloco livre = %d\n", bloco_livre->Registrador.Flag.tamanho);

	//Remove o bloco da lista de blocos 
 	for (aux; aux!=NULL; aux = aux->prox)
 		if ((bloco_livre->Registrador.Flag.tamanho == aux->Registrador.Flag.tamanho) && aux->Registrador.Flag.livre ==1)
 			break;

	if(aux == NULL)
		return 1;

	/*if (*(Gerencia.base_dados) == aux) // testa se é o primeiro elemento 
 	{
 		printf("foi o fu=isrt\n");
 		*(Gerencia.base_dados) = aux->prox;
 	}
 	else{
 		printf("%d\n", aux->ant->Registrador.Flag.tamanho);
 		aux->ant->prox = aux->prox;
 		printf("hou\n");
 	}
 	if (aux->prox != NULL) // testa se é o último elemento 
 		aux->prox->ant = aux->ant;
	*/

	//Adiciona o bloco na lista de blocos livres
 	if((*(Gerencia.blocos_livres) == NULL)){
		(*(Gerencia.blocos_livres)) = bloco_livre;
	}

	else{
		printf("hahahah\n");
		bloco_livre->prox  = (*(Gerencia.blocos_livres));
		(*(Gerencia.blocos_livres)) = bloco_livre; 
	}

	//printf("Bloco Livre->tamanho  === %d\n", (*(Gerencia.blocos_livres))->Registrador.Flag.tamanho);
 	return 0;
}

void MyMallocGerency(){

	printf("Lista de blocos alocados:\n");

	Bloco aux = (*(Gerencia.base_dados));
	for(aux; aux!= NULL; aux = aux->prox){

		printf("Bloco de tamanho: %d\n", aux->Registrador.Flag.tamanho);
		printf("Livre: %d\n", aux->Registrador.Flag.livre);
	}

	aux = (*(Gerencia.blocos_livres));
	if(aux){
		printf("Lista de blocos livres: \n");

		for(aux; aux!= NULL; aux = aux->prox)
		{
			printf("Bloco de tamanho: %d\n", aux->Registrador.Flag.tamanho);
		}
	}
}