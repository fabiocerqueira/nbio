#include "Python.h"

// Assinatura do nosso modulo
void initnbio(void);

// Função principal que embute o python dentro do C.
int main(int argc, char **argv) {
	// Inicializa o Interpretador python embutido
	Py_Initialize();
	// Adiciona o nosso modulo
	initnbio();
	// Sai do interpretador embutido.
	Py_Exit(0);
	return 0;
}

/*
Aqui vão as funções do nosso módulo. Todas que fiz seguiram um modelo.
*/

/*
Note que o nome da função sempre vem com [NOME_DO_MODULO]_[NOME_DA_FUNCAO].
Esse metodo apenas da um 'hello world' usando o printf e o print do python.
*/
static PyObject *nbio_hello_world(PyObject *self, PyObject* args)
{
	printf("[C] Opa, aqui eh C, e usei o printf.\n");

	PyRun_SimpleString("import time\n");
	PyRun_SimpleString("print '[Py] Aqui eh Python:',time.time()");

	printf("\n[C] Aqui é C também.\n");
	return Py_None;
}

static PyObject *nbio_argumento_numeros(PyObject *self, PyObject* args)
{
	long numero1, numero2, soma;

	/* A função PyArg_ParseTuple() é responsável pela transição de argumentos
	das funções em python para tipos C.
	*/
	if (!PyArg_ParseTuple(args, "ll:addi", &numero1, &numero2)) {
		return NULL;
	}
	// Note que já trato numero1 e numero2 como long C.
	printf("[C] Primeiro Argumento: %d\n",numero1);
	printf("[C] Segundo Argumento: %d\n",numero2);

	soma = numero1 + numero2;

	// Transformando e retornando o resultado da soma em um long Python.
	return PyLong_FromUnsignedLong(soma);
}

static PyObject *nbio_argumento_string(PyObject *self, PyObject* args)
{
	char *str1, str2[15] = "str_c";

	if (!PyArg_ParseTuple(args, "s:addi", &str1)) {
		return NULL;
	}
	printf("[C] Recebido array de caracteres: %s\n",str1);
	printf("[C] Contatenacao usando strcat() com: %s\n",str2);
	strcat(str2, str1);
	printf("[C] Retornando resultado pra objeto string python: %s\n",str2);
	return PyString_FromString(str2);
}

/*
Aqui vai uma estrutura com todas as funções dos módulos. Observe que é convenção usar
o nome da função python igual a função C.
*/

static PyMethodDef nbio_methods[] = {
	{"hello_world",nbio_hello_world,METH_NOARGS, "Documentação do hello world!"},
	{"argumento_numeros",nbio_argumento_numeros,METH_VARARGS, "Documentação do argumento_numeros"},
	{"argumento_string",nbio_argumento_string,METH_VARARGS, "Documentação do argumento_numeros"},
	{NULL, NULL} //sentinela
};

/*
Função para importar o módulo dentro do interpretador embutido, que foi inicializado no main.
*/
void initnbio(void)
{
	PyImport_AddModule("nbio");
	Py_InitModule("nbio", nbio_methods);
}
