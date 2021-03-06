#include "interfacePrincipal.hpp"
#include <iomanip>
#include <algorithm> 

string InterfacePrincipal::getString(string mensagem) {
    string entrada;
    while(true) {
        cout << mensagem;
        getline(cin, entrada);
        if(entrada.empty() || entrada[0] == ' ')
            cout << endl << "Entrada inválida" << endl; 
        else
            break; 
    }
    return entrada;
}

int InterfacePrincipal::getInt(string mensagem, int min, int max) {
    int valor;
    bool loop = true;
    while(loop) {
        cout << mensagem;
        loop = false;
        cin >> valor;
        if(cin.fail() || valor > max || valor < min) {
            cin.clear();
            cout << endl << "Entrada inválida" << endl;
            loop = true;
        }
        cin.ignore(32767, '\n');
    }
    return valor;
}

double InterfacePrincipal::getDouble(string mensagem, double min) {
    double valor;
    bool loop = true;
    while(loop) {
        cout << mensagem;
        loop = false;
        cin >> valor;
        if(cin.fail() || valor < min) {
            cin.clear();
            cout << "Entrada inválida" << endl;
            loop = true;
        }
        cin.ignore(32767, '\n');
    }
    return valor;
}

void InterfacePrincipal::spam(string mensagem){
    system("clear||cls"); 
    cout << mensagem << endl << endl; 

}

InterfacePrincipal::InterfacePrincipal(){
    system("clear||cls");
    hrIntervalo = 0;
    minIntervalo = 0; 
}

void InterfacePrincipal::menuPrincipal() {
    while(true){
        cout << "(1) Nova tarefa" << endl;
        cout << "(2) Ver tarefas" << endl;
        cout << "(3) Excluir tarefa" << endl; 
        cout << "(4) Excluir todas as tarefas" << endl; 
        cout << "(5) Definir intervalo entre tarefas (Atual: " << formatarHorario(hrIntervalo, minIntervalo) << ")" << endl;
        cout << "(6) Mostrar conjunto mais rentável de tarefas" << endl;
        cout << "(0) Fechar programa" << endl; 
        int escolha = getInt("", 0,6);
        system("clear||cls");
        if(escolha == 1)
            novaTarefa(); 
        else if(escolha == 2)
            verTarefas(); 
        else if(escolha == 3)
            excluirTarefa();
        else if(escolha == 4)
            limparTarefas();
        else if(escolha == 5)
            novoIntervalo(); 
        else if(escolha == 6)
            conjuntoRentavel();
        else
            break; 
    }
}

void InterfacePrincipal::novaTarefa(){
    string descricao = getString("Descrição da tarefa: ");
    int hi = getInt("Hora inicial: ", 0, 23);
    int mi = getInt("Minuto inicial: ", 0, 59);
    int hf = getInt("Hora final: ", 0, 23);
    int mf = getInt("Minuto final: ", 0, 59);
    if(hi > hf || (hi == hf && mi >= mf)) {
        spam("Tarefa não cadastrada (horário inválido)");
        return;
    }
    double renda = getDouble("Renda da tarefa: ", 0.01);
    tarefas.push_back(Tarefa(descricao, hi, hf, mi, mf, renda));
    spam("Tarefa cadastrada com sucesso");
}

void InterfacePrincipal::verTarefas(){
    if(tarefas.empty())
        spam("Não há tarefas cadastradas");
    for(int i = 0; i < (int)tarefas.size(); i++) {
        tarefas[i].mostrarInfos();
        cout << setw(50) << setfill('-') << "\n\n";
    }
}

void InterfacePrincipal::novoIntervalo(){
    cout << "Redefinindo o horário de intervalo" << endl << endl;
    hrIntervalo = getInt("Hora: ", 0, 23);
    minIntervalo = getInt("Minuto: ", 0, 59);
    spam("O intervalo foi atualizado para " + formatarHorario(hrIntervalo, minIntervalo)); 
}

void InterfacePrincipal::excluirTarefa() {
    if(tarefas.empty()) {
        spam("Não há tarefas cadastradas");
        return;
    }
    for(int i = 0; i < (int)tarefas.size(); i++)
        cout << "(" << i + 1 << ") " << tarefas[i].getDescricao() << endl;
    int id = getInt("\nNúmero da tarefa para excluir: ", 1, tarefas.size());
    tarefas.erase(tarefas.begin() + id - 1);
    spam("Tarefa excluída com sucesso");
}

void InterfacePrincipal::limparTarefas() {
    tarefas.clear();
    spam("Todas as tarefas foram excluídas");
}

void InterfacePrincipal::ordenarTarefas(vector <Tarefa> &tarefas){
    sort(tarefas.begin(), tarefas.end(), [](Tarefa &a, Tarefa &b){
            if(a.getHrF() < b.getHrF())
                return true;
            if(a.getHrF() == b.getHrF())
                return (a.getminF() <= b.getminF());
            return false;
        }
    );
}

string InterfacePrincipal::formatarHorario(int h, int m) {
    string intervalo = ""; 
    if(h < 10)
         intervalo += "0";
    intervalo += (to_string(h) + ":");
    if(m < 10)
        intervalo += "0";
    intervalo += (to_string(m));  
    return intervalo; 
}

int InterfacePrincipal::computeP(vector <Tarefa> &tarefas, int j){
    for(int i = j - 1; i >= 0; i--) {
        if(isCompativel(tarefas[i], tarefas[j]))
            return i;  
    }
    return -1;
}

bool InterfacePrincipal::isCompativel(Tarefa &a, Tarefa &b){
    int horaDiponivel, minDisponivel; 
    horaDiponivel = a.getHrF() + hrIntervalo; 
    minDisponivel = a.getminF() + minIntervalo; 
    if(minDisponivel > 59){
        horaDiponivel+=1;
        minDisponivel = minDisponivel%60; 
    }
    if(b.getHrI() < horaDiponivel)
        return false; 
    else if(b.getHrI() == horaDiponivel){
        if(b.getMinI() < minDisponivel)
            return false; 
    }
    return true; 
}

double InterfacePrincipal::computeOptRenda(vector <Tarefa> &tarefas, vector <double> &M, vector <int> &p, int j) {
    if(j == -1)
        return 0;
    if(M[j] == -1)
        M[j] = max(tarefas[j].getRenda() + computeOptRenda(tarefas, M, p, p[j]), computeOptRenda(tarefas, M, p, j - 1));
    return M[j];
}

void InterfacePrincipal::conjuntoRentavel() {
    if(tarefas.empty()) {
        spam("Não há tarefas cadastradas");
        return;
    }
    vector <Tarefa> tarefas = this->tarefas;
    vector <double> M(tarefas.size(), -1);
    vector <int> p(tarefas.size(), -1);
    ordenarTarefas(tarefas);
    for(int i = tarefas.size() - 1; i > 0 ; i--)
        p[i] = computeP(tarefas, i);
    cout << fixed << setprecision(2) << "O maior salário possível é: R$" << computeOptRenda(tarefas, M, p, tarefas.size() - 1) << endl << endl;
    findSolution(tarefas, M, p, tarefas.size() - 1); 
}

void InterfacePrincipal::findSolution(vector <Tarefa> &tarefas, vector <double> &M, vector <int> &p, int j){
    if(j == -1)
        return;
    if(tarefas[j].getRenda() + M[p[j]] > M[j-1]){
        tarefas[j].mostrarInfos();
        findSolution(tarefas, M, p, p[j]); 
    } else
        findSolution(tarefas, M, p, j-1); 
}
