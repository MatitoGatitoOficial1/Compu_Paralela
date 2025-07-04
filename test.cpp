#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

// ─────────seccion de normalización ─────────
string norm(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return tolower(c); });
    s.erase(remove_if(s.begin(), s.end(),
                      [](unsigned char c){ return c==' '||c=='\t'||c=='_'; }),
            s.end());
    const vector<pair<string,string>> repl = {
        {u8"á","a"},{u8"é","e"},{u8"í","i"},{u8"ó","o"},{u8"ú","u"},{u8"ñ","n"}
    };
    for (auto& p : repl) {
        size_t pos = 0;
        while ((pos = s.find(p.first, pos)) != string::npos) {
            s.replace(pos, p.first.size(), p.second);
            pos += p.second.size();
        }
    }
    return s;
}

vector<string> split_sc(const string& line) {
    vector<string> out; string cur; bool inq = false;
    for (char c : line) {
        if (c == '"') inq = !inq;
        else if (c == ';' && !inq) {
            if (!cur.empty() && cur.front()=='"' && cur.back()=='"')
                cur = cur.substr(1, cur.size()-2);
            out.push_back(cur); cur.clear();
        } else cur.push_back(c);
    }
    if (!cur.empty() && cur.front()=='"' && cur.back()=='"')
        cur = cur.substr(1, cur.size()-2);
    out.push_back(cur);
    return out;
}

// mapeos texto → índice
int idEspecie(const string& s) {
    string v = norm(s);
    if (v=="humano"||v=="humana"||v=="hombrebestia") return 0;
    if (v=="elfo"||v=="elfa"||v=="elfico"||v=="elfica") return 1;
    if (v=="enano"||v=="enana") return 2;
    if (v=="orco"||v=="orc") return 3;
    return -1;
}
int idGenero(const string& s) {
    string v = norm(s);
    if (v=="m"||v=="masculino"||v=="macho") return 0;
    if (v=="f"||v=="femenino"||v=="hembra") return 1;
    return 2;
}
int calcularEdad(const string& col) {
    if (col.size()<4 || !isdigit(col[0])) return -1;
    try {
        int y = stoi(col.substr(0,4));
        int edad = 2025 - y;
        return (edad>=0 && edad<100)? edad : -1;
    } catch (...) { return -1; }
}

// ──────── estructuras globales ────────
array<uint64_t,10> gEstratos{};
array<array<array<uint64_t,100>,3>,4> gMat{};

// ─────────── escribir CSV ────────────
void guardarEstratosCSV(uint64_t total) {
    ofstream f("estratos.csv");
    f << "estrato,count,percentage\n";
    for (int e=0; e<10; ++e) {
        double pct = total ? (gEstratos[e]*100.0/total) : 0.0;
        f << e << ',' << gEstratos[e] << ',' << pct << '\n';
    }
}
void guardarMatrizCSV() {
    static const array<string,4> sp = {"Humano","Elfo","Enano","Orco"};
    static const array<string,3> gn = {"M","F","O"};
    ofstream f("especie_genero_edad.csv");
    f << "especie,genero,edad,count\n";
    for (int s=0; s<4; ++s)
        for (int g=0; g<3; ++g)
            for (int a=0; a<100; ++a)
                if (gMat[s][g][a])
                    f << sp[s] << ',' << gn[g] << ',' << a << ',' << gMat[s][g][a] << '\n';
}

int main(int argc, char* argv[]) {
    if (argc>1) { int h=atoi(argv[1]); if(h>0) omp_set_num_threads(h); }
    cerr << "Usando " << omp_get_max_threads() << " hilos\n";

    ifstream fin("eldoria.csv");
    if (!fin) { cerr<<"No se encontró eldoria.csv\n"; return 1; }

    // localizar columnas
    string header; getline(fin, header);
    auto hTok = split_sc(header);
    vector<string> nHead; for (auto& h : hTok) nHead.push_back(norm(h));

    auto findCol = [&](string key) {
        key = norm(key);
        for (size_t i=0; i<nHead.size(); ++i) if (nHead[i]==key) return i;
        return static_cast<size_t>(-1);
    };
    size_t icpO = findCol("cp origen");
    size_t iEsp = findCol("especie");
    size_t iGen = findCol("genero");
    size_t iAn  = findCol("año nac");
    if (iAn==(size_t)-1) iAn=findCol("ano nac");
    if (iAn==(size_t)-1) iAn=findCol("fecha nacimiento");
    if (icpO==(size_t)-1||iEsp==(size_t)-1||iGen==(size_t)-1||iAn==(size_t)-1) {
        cerr<<"No se hallaron todas las columnas requeridas\n"; return 1;
    }

    // procesamiento por bloques
    const size_t BLOCK = 100000;
    vector<string> lines; lines.reserve(BLOCK);
    string line; uint64_t total = 0;
    auto t0 = chrono::high_resolution_clock::now();

    while (true) {
        lines.clear();
        for (size_t i=0; i<BLOCK && getline(fin,line); ++i)
            if (!line.empty()) lines.push_back(move(line));
        if (lines.empty()) break;

        #pragma omp parallel default(none) \
            shared(lines, icpO, iEsp, iGen, iAn, total, gEstratos, gMat)
        {
            array<uint64_t,10> locEstr{};
            array<array<array<uint64_t,100>,3>,4> locMat{};

            #pragma omp for schedule(static)
            for (size_t i=0; i<lines.size(); ++i) {
                auto col = split_sc(lines[i]);
                if (col.size()<=max({icpO,iEsp,iGen,iAn})) continue;

                if (!col[icpO].empty() && isdigit(col[icpO][0])) {
                    int e = col[icpO][0]-'0';
                    if (e>=0 && e<10) locEstr[e]++;
                }
                int s = idEspecie(col[iEsp]);
                int g = idGenero(col[iGen]);
                int edad = calcularEdad(col[iAn]);
                if (s>=0 && g>=0 && g<3 && edad>=0) locMat[s][g][edad]++;
            }

            #pragma omp critical
            {
                for (int e=0; e<10; ++e) gEstratos[e] += locEstr[e];
                for (int s=0; s<4; ++s) for (int g=0; g<3; ++g)
                    for (int a=0; a<100; ++a) gMat[s][g][a] += locMat[s][g][a];
                total += accumulate(locEstr.begin(), locEstr.end(), uint64_t(0));
            }
        }
        if (fin.eof()) break;
    }

    cerr << "Tiempo total: "
         << chrono::duration<double>(chrono::high_resolution_clock::now()-t0).count()
         << " s\n";

    guardarEstratosCSV(total);
    guardarMatrizCSV();

    cout << "✓ estratos.csv y especie_genero_edad.csv generados.\n";
    return 0;
}
