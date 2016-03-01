/**
 * Alunos: Sandys Nunes e Vitor Naito
 */

#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <streambuf>

using namespace std;

// variaveis globais
// valores da função w
// a inicialização de seus valores será feita antes de executar o algoritmo principal
int igual_value;
int diferente_value;
int espaco_value;

//constantes
static const char * const MSG_FOMATO_ESPERADO =
		"Use o seguinte formato:\n<nome executavel> [-g,-l] -u <arquivo 1a. sequencia> -v <arquivo 2a. sequencia> -i <w(a,a)> -d <w(a,b)> -e <w(-,b) ou w(a,-)>\n";
static const int TAMANHO_ESPERADO_ARGUMENTOS = 12;
static const int CARACTERES_POR_LINHA = 30;

//irá definir a navegação na matrix de alinhamento
enum Direcao {
	ACIMA = 'A', ESQUERDA = 'E', DIAGONAL = 'D', INDEFINIDA = 'I'
};

//Classe que define uma celula da matriz usada pelo algoritmo,
//contendo a direção e o valor calculado
class Celula {
public:

	Celula(Direcao direcao, int valor) {
		this->direcao = direcao;
		this->valor = valor;
	}

	Celula() {
		this->direcao = INDEFINIDA;
		this->valor = 0;
	}

	//getters e setters
	Direcao getDirecao() const {
		return direcao;
	}

	void setDirecao(Direcao direcao) {
		this->direcao = direcao;
	}

	int getValor() const {
		return valor;
	}

	void setValor(int valor) {
		this->valor = valor;
	}

	//usado pela função max
	friend bool operator <(const Celula& left, const Celula& right) {
		return left.valor < right.valor;
	}

private:
	Direcao direcao;
	int valor;

};

//cabeçalhos de funções
char *getCmdOption(char**, char**, const string&);
bool cmdOptionExists(char**, char**, const string&);
const char *fastaFileToCharArray(const char*);
int w(char, char);
void imprimirAlinhamento(const string&, const string&, int);
void construirMatrizAlinhamento(const string&, const string &, bool);

/**
 * Main
 */
int main(int argc, char * argv[]) {

	if (argc != TAMANHO_ESPERADO_ARGUMENTOS) {
		printf(MSG_FOMATO_ESPERADO);
		return EXIT_FAILURE;
	}

	//faz a leitura dos argumentos
	bool isLocal = cmdOptionExists(argv, argv + argc, "-l");

	char * fileNameSeq1 = getCmdOption(argv, argv + argc, "-u");
	char * fileNameSeq2 = getCmdOption(argv, argv + argc, "-v");

	igual_value = atoi(getCmdOption(argv, argv + argc, "-i"));
	diferente_value = atoi(getCmdOption(argv, argv + argc, "-d"));
	espaco_value = atoi(getCmdOption(argv, argv + argc, "-e"));

	//executa o algoritmo
	construirMatrizAlinhamento(fastaFileToCharArray(fileNameSeq1), fastaFileToCharArray(fileNameSeq2), isLocal);

	return EXIT_SUCCESS;
}

//obtem o valor de uma opção de argumento.
char *getCmdOption(char **begin, char **end, const string &option) {
	char **itr = find(begin, end, option);
	if (itr != end && ++itr != end) {
		return *itr;
	}
	return 0;
}

//verifica se determinado argumento foi passado no console
bool cmdOptionExists(char **begin, char **end, const string &option) {
	return find(begin, end, option) != end;
}

//Dado um arquivo no formato FASTA devolve uma única string.
//A primeira linha do arquivo é ignorada.
const char *fastaFileToCharArray(const char *fileName) {
	ifstream file(fileName);
	string str;
	string file_contents;
	int line_index = 0;
	while (getline(file, str)) {
		if (line_index != 0) {
			file_contents += str;
		}
		line_index++;
	}
	return file_contents.c_str();
}

//função de pontuação
int w(char u, char v) {
	return (u == v) ? igual_value : (u == '-' || v == '-') ? espaco_value : diferente_value;
}

//imprime o alinhamento criado
void imprimirAlinhamento(const string& s, const string& t, int similaridade) {

	//calcula quantas porções de alinhamentos serão impressos dado o limite de 30 caracteres
	int partes = s.length() / CARACTERES_POR_LINHA;
	if (s.length() % CARACTERES_POR_LINHA > 0) {
		partes++;
	}
	for (int i = 0; i < partes; i++) {
		string s_partial = s.substr(i * CARACTERES_POR_LINHA, CARACTERES_POR_LINHA);
		string t_partial = t.substr(i * CARACTERES_POR_LINHA, CARACTERES_POR_LINHA);
		//imprime sequencia 1
		cout << "seq1: " << s_partial << endl;
		//imprime caracteres especiais da linha do meio (match, mismatch e espaço)
		cout << "      ";
		for (int j = 0; j < int(s_partial.length()); j++) {
			if (t_partial.at(j) == '-' || s_partial.at(j) == '-') {
				//possui traço
				cout << " ";
				continue;
			} else if (s_partial.at(j) == t_partial.at(j)) {
				//caracteres iguais
				cout << "|";
			} else {
				//caracteres diferentes
				cout << "!";
			}
		}
		cout << endl;
		//imprime sequencia 2
		cout << "seq2: " << t_partial << endl;
		if (i + 1 < partes) {
			cout << endl;
		}
	}
	cout << "Similaridade: " << similaridade << endl;
}

void construirMatrizAlinhamento(const string &s, const string &t, bool isLocal) {

	int linhas = s.length() + 1;
	int colunas = t.length() + 1;

	//Alocação da matriz
	Celula **M = new Celula*[linhas];
	for (int i = 0; i < linhas; i++) {
		M[i] = new Celula[colunas]();
	}

	for (int k = 1; k < colunas; k++) {
		M[0][k].setValor(k * w('-', t.at(k - 1)));
		M[0][k].setDirecao(ESQUERDA);
	}

	for (int k = 1; k < linhas; k++) {
		M[k][0].setValor(k * w(s.at(k - 1), '-'));
		M[k][0].setDirecao(ACIMA);
	}

	for (int i = 1; i < linhas; i++) {
		for (int j = 1; j < colunas; j++) {
			Celula acima(ACIMA, M[i - 1][j].getValor() + w(s.at(i - 1), '-'));
			Celula diagonal(DIAGONAL, M[i - 1][j - 1].getValor() + w(s.at(i - 1), t.at(j - 1)));
			Celula esquerda(ESQUERDA, M[i][j - 1].getValor() + w('-', t.at(j - 1)));

			if (isLocal) {
				Celula zero(INDEFINIDA, 0);
				M[i][j] = max(max(diagonal, max(acima, esquerda)), zero);
			} else {
				M[i][j] = max(diagonal, max(acima, esquerda));
			}

		}
	}

	//strings que serão os alinhamentos resultantes
	string _s = "";
	string _t = "";
	int similaridade;

	//controi alinhamento local
	if (isLocal) {

		//busca a posição do maior valor na matriz
		Celula *maior = NULL;
		int linhaMaior, colunaMaior;
		for (int i = 0; i < linhas; i++) {
			for (int j = 0; j < colunas; j++) {
				if (maior == NULL || M[i][j].getValor() > maior->getValor()) {
					maior = &M[i][j];
					linhaMaior = i;
					colunaMaior = j;
				}
			}
		}

		similaridade = M[linhaMaior][colunaMaior].getValor();

		for (int i = linhaMaior, j = colunaMaior; i > 0 || j > 0;) {
			Celula cell = M[i][j];
			if (cell.getValor() == 0) {
				break;
			}
			if (cell.getDirecao() == DIAGONAL) {
				_s += s.at(--i);
				_t += t.at(--j);
			} else if (cell.getDirecao() == ACIMA) {
				_s += s.at(--i);
				_t += "-";
			} else if (cell.getDirecao() == ESQUERDA) {
				_s += "-";
				_t += t.at(--j);
			}
		}

	} else { //controi alinhamento global
		similaridade = M[linhas - 1][colunas - 1].getValor();
		for (int i = linhas - 1, j = colunas - 1; i > 0 || j > 0;) {
			Celula cell = M[i][j];
			if (cell.getDirecao() == DIAGONAL) {
				_s += s.at(--i);
				_t += t.at(--j);
			} else if (cell.getDirecao() == ACIMA) {
				_s += s.at(--i);
				_t += "-";
			} else if (cell.getDirecao() == ESQUERDA) {
				_s += "-";
				_t += t.at(--j);
			}
		}
	}

	//inverte os alinhamentos
	reverse(_s.begin(), _s.end());
	reverse(_t.begin(), _t.end());

	imprimirAlinhamento(_s, _t, similaridade);

	//desaloca matriz
	for (int i = 0; i < linhas; i++) {
		delete[] M[i];
	}
	delete[] M;
}

