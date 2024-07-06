#include <bits/stdc++.h>

using namespace std;

struct Gate;
struct Wire;

struct Gate
{
    string label; // 導線名稱
    Wire *ZN = NULL; //輸出
    Wire *A1 = NULL; // A1輸入
    Wire *A2 = NULL; // A2輸入
    int gate_type = -1; // 0代表INVX1, 1代表NOR2X1, 2代表NANDX1
    vector<Wire *> path; // 它前面所有的路徑
    double o_cap = 0;    // 輸出電容
    double trans = -1;   // transition time
    double delay = -1;   // delay time
    double total_delay = 0; // 總delay
};
typedef struct Gate Gate;

struct Wire
{
    int value = -1;      // 這條線傳輸的值
    string label;        // 導線名稱
    Gate *pre = NULL;    // wire的前面gate是哪個
    bool OUT = false;  // 若是true，代表是output導線
    vector<Gate *> next; // 輸出gate有哪些
};
typedef struct Wire Wire;

const vector<double> all_cap = {0.001400, 0.003000, 0.006200, 0.012500, 0.025100, 0.050400, 0.101000};
const vector<double> all_in_trans = {0.0208, 0.0336, 0.06, 0.1112, 0.2136, 0.4192, 0.8304};


const double NOR2X1_A1_capacitance = 0.0105008;
const double NOR2X1_A2_capacitance = 0.0108106;
const vector<vector<double>> NOR2X1_cell_rise{
    {0.023104, 0.026347, 0.032439, 0.037211, 0.050187, 0.075629, 0.108930},
    {0.027062, 0.029858, 0.036397, 0.042338, 0.055823, 0.083858, 0.119420},
    {0.034333, 0.037002, 0.043649, 0.050318, 0.064815, 0.089926, 0.138270},
    {0.048838, 0.051443, 0.053267, 0.064115, 0.083493, 0.112050, 0.170690},
    {0.076172, 0.077668, 0.081485, 0.089780, 0.112630, 0.149040, 0.203590},
    {0.133510, 0.134860, 0.138470, 0.146350, 0.162780, 0.209740, 0.279490},
    {0.247770, 0.248880, 0.252340, 0.259710, 0.275960, 0.309680, 0.403930}};
const vector<vector<double>> NOR2X1_cell_fall{
    {0.017693, 0.020473, 0.024855, 0.029528, 0.034750, 0.038561, 0.030505},
    {0.019856, 0.022665, 0.027792, 0.033415, 0.039820, 0.045554, 0.039165},
    {0.024076, 0.026936, 0.032932, 0.040339, 0.048928, 0.057779, 0.054661},
    {0.032280, 0.035141, 0.041568, 0.051796, 0.064313, 0.069003, 0.080192},
    {0.048376, 0.051227, 0.057825, 0.070186, 0.089026, 0.098520, 0.118860},
    {0.078389, 0.080130, 0.090011, 0.091689, 0.112830, 0.150920, 0.179760},
    {0.142020, 0.143720, 0.147350, 0.154830, 0.170740, 0.213410, 0.290100}};
const vector<vector<double>> NOR2X1_rise_transition{
    {0.047372, 0.047388, 0.052910, 0.069558, 0.079757, 0.112950, 0.162980},
    {0.055454, 0.057237, 0.061359, 0.076048, 0.089491, 0.122180, 0.178600},
    {0.070803, 0.069305, 0.077650, 0.088390, 0.107060, 0.158050, 0.204620},
    {0.097384, 0.095745, 0.106670, 0.113940, 0.137200, 0.176410, 0.237050},
    {0.166060, 0.163680, 0.167160, 0.170000, 0.185930, 0.235810, 0.338230},
    {0.296300, 0.290000, 0.290770, 0.299970, 0.295960, 0.342040, 0.431200},
    {0.556050, 0.540720, 0.539690, 0.541850, 0.563770, 0.570260, 0.652330}};
const vector<vector<double>> NOR2X1_fall_transition{
    {0.017800, 0.020672, 0.025912, 0.038106, 0.054758, 0.087109, 0.140530},
    {0.022110, 0.024126, 0.029235, 0.043016, 0.061952, 0.094784, 0.152170},
    {0.029050, 0.032076, 0.035248, 0.050710, 0.073218, 0.110450, 0.170340},
    {0.045602, 0.046546, 0.049838, 0.065112, 0.089226, 0.138180, 0.220760},
    {0.077240, 0.077419, 0.079545, 0.087836, 0.115320, 0.180490, 0.259710},
    {0.135930, 0.133980, 0.141400, 0.146810, 0.168520, 0.225280, 0.343700},
    {0.259810, 0.254610, 0.253670, 0.262630, 0.277020, 0.332350, 0.443690}};
unordered_map<string, Wire *> N_W;
const double NANDX1_A1_capacitance = 0.00683597;
const double NANDX1_A2_capacitance = 0.00798456;
const vector<vector<double>> NANDX1_cell_rise{
    {0.020844, 0.024310, 0.030696, 0.039694, 0.048205, 0.072168, 0.101880},
    {0.024677, 0.027942, 0.035042, 0.045467, 0.054973, 0.082349, 0.115390},
    {0.032068, 0.035394, 0.042758, 0.055361, 0.065991, 0.090936, 0.138470},
    {0.046811, 0.049968, 0.057164, 0.064754, 0.086481, 0.116760, 0.157440},
    {0.073919, 0.078805, 0.080873, 0.091007, 0.116550, 0.157900, 0.214480},
    {0.131620, 0.133630, 0.138300, 0.147930, 0.168500, 0.220320, 0.300540},
    {0.246610, 0.248350, 0.252940, 0.262210, 0.282000, 0.324170, 0.427830}};
const vector<vector<double>> NANDX1_cell_fall{
    {0.020382, 0.023257, 0.027894, 0.033548, 0.035740, 0.041790, 0.044857},
    {0.024029, 0.026956, 0.032263, 0.039221, 0.042085, 0.050402, 0.056660},
    {0.031269, 0.034119, 0.039869, 0.049176, 0.053737, 0.065333, 0.075780},
    {0.045259, 0.047829, 0.053987, 0.057975, 0.072805, 0.089369, 0.109580},
    {0.071393, 0.072867, 0.076097, 0.082901, 0.102690, 0.130350, 0.160910},
    {0.127140, 0.128510, 0.131750, 0.138250, 0.152920, 0.192220, 0.245200},
    {0.238460, 0.239670, 0.242900, 0.249230, 0.262520, 0.293170, 0.371160}};
const vector<vector<double>> NANDX1_rise_transition{
    {0.030236, 0.032250, 0.038219, 0.052208, 0.080479, 0.107190, 0.151470},
    {0.039514, 0.039353, 0.046985, 0.061163, 0.087011, 0.116970, 0.175150},
    {0.051087, 0.053959, 0.058155, 0.070827, 0.109340, 0.152230, 0.206410},
    {0.085969, 0.084220, 0.092301, 0.096621, 0.129250, 0.199410, 0.265970},
    {0.153720, 0.147050, 0.154690, 0.160300, 0.179550, 0.248520, 0.357890},
    {0.286690, 0.280220, 0.281210, 0.288880, 0.278630, 0.331600, 0.482560},
    {0.552500, 0.535920, 0.534410, 0.535030, 0.556830, 0.542710, 0.659570}};
const vector<vector<double>> NANDX1_fall_transition{
    {0.027802, 0.028745, 0.035327, 0.047243, 0.067704, 0.097889, 0.151650},
    {0.034160, 0.036630, 0.041513, 0.054197, 0.079795, 0.110130, 0.165450},
    {0.048764, 0.050279, 0.053838, 0.065970, 0.098021, 0.133030, 0.195920},
    {0.076954, 0.077267, 0.077694, 0.089180, 0.115280, 0.172860, 0.238320},
    {0.133070, 0.133160, 0.135080, 0.141060, 0.162490, 0.213700, 0.324300},
    {0.247090, 0.241360, 0.245160, 0.250640, 0.263980, 0.319640, 0.446030},
    {0.474440, 0.460220, 0.462840, 0.466540, 0.481330, 0.489290, 0.603170}};

vector <Wire *> input_wire;

const double INVX1_I_capacitance = 0.0109115;
const vector<vector<double>> INVX1_cell_fall{
    {0.015975, 0.018648, 0.022904, 0.028037, 0.031885, 0.041402, 0.053623},
    {0.018156, 0.021149, 0.026094, 0.032287, 0.036680, 0.048075, 0.062383},
    {0.022594, 0.025590, 0.031664, 0.039739, 0.044932, 0.059061, 0.077439},
    {0.031149, 0.034095, 0.040822, 0.051786, 0.060032, 0.076833, 0.102310},
    {0.047947, 0.047918, 0.057692, 0.063035, 0.082424, 0.107350, 0.140340},
    {0.079511, 0.081269, 0.085301, 0.093628, 0.116410, 0.154500, 0.202000},
    {0.146040, 0.147720, 0.151580, 0.159670, 0.176810, 0.223720, 0.298480}};
const vector<vector<double>> INVX1_cell_rise{
    {0.013379, 0.016044, 0.019907, 0.022481, 0.031726, 0.040185, 0.050385},
    {0.015250, 0.018376, 0.022972, 0.026053, 0.037050, 0.047418, 0.059875},
    {0.018871, 0.022403, 0.028108, 0.032610, 0.045789, 0.059925, 0.076608},
    {0.025970, 0.029306, 0.033033, 0.042608, 0.054479, 0.080008, 0.103940},
    {0.040047, 0.040538, 0.045817, 0.058067, 0.076355, 0.099330, 0.146830},
    {0.066295, 0.068558, 0.073341, 0.083717, 0.108210, 0.143970, 0.189040},
    {0.122230, 0.124410, 0.129200, 0.138630, 0.159470, 0.208640, 0.279200}};
const vector<vector<double>> INVX1_rise_transition{
    {0.017522, 0.021682, 0.028803, 0.041672, 0.052872, 0.081109, 0.128370},
    {0.021980, 0.024661, 0.033109, 0.046939, 0.060235, 0.088586, 0.137120},
    {0.029796, 0.032708, 0.038207, 0.056464, 0.078060, 0.099037, 0.155610},
    {0.044871, 0.046301, 0.051610, 0.068661, 0.102220, 0.124680, 0.182110},
    {0.076836, 0.076772, 0.076054, 0.089262, 0.128370, 0.186760, 0.232160},
    {0.141700, 0.141210, 0.141980, 0.142120, 0.171260, 0.244100, 0.354290},
    {0.270810, 0.266810, 0.269430, 0.274220, 0.274290, 0.332280, 0.473410}};
const vector<vector<double>> INVX1_fall_transition{
    {0.017349, 0.017893, 0.024178, 0.035104, 0.051821, 0.075654, 0.123160},
    {0.018614, 0.022157, 0.027649, 0.040185, 0.059238, 0.085087, 0.133440},
    {0.027419, 0.029372, 0.034899, 0.049446, 0.072893, 0.104290, 0.152460},
    {0.043657, 0.044863, 0.049056, 0.060723, 0.091025, 0.132310, 0.185960},
    {0.076530, 0.073661, 0.079082, 0.087928, 0.120850, 0.173160, 0.248810},
    {0.137490, 0.135180, 0.137930, 0.149130, 0.165850, 0.229130, 0.339420},
    {0.265330, 0.262480, 0.265530, 0.265920, 0.286880, 0.331030, 0.452580}};

vector<Gate *> totalgate;


const vector<string> dis(const string &str, const string &pattern) {
    vector<std::string> result;
    istringstream stream(str);
    string token;

    while (getline(stream, token, pattern[0])) {
        result.push_back(token);
    }

    return result;
}

list<string> OUT_name;
void store_wire(string line)
{
    int pos = 0;
    string substring = "";

    if (line.find("wire") != string::npos)
    {
        pos = line.find("wire");
        substring = line.substr(pos + 5);
        vector<string> wires = dis(substring, ",");
        for (int i = 0; i < wires.size(); i++)
        {
            wires[i].erase(remove_if(wires[i].begin(), wires[i].end(), ::isspace), wires[i].end());
            Wire *w = new Wire();
            w->label = wires[i];
            N_W[wires[i]] = w;
        }
    }
    
    else if (line.find("input") != string::npos)
    {
        pos = line.find("input");
        
        substring = line.substr(pos + 6);
        vector<string> wires = dis(substring, ",");
        for (int i = 0; i < wires.size(); i++)
        {
            wires[i].erase(std::remove_if(wires[i].begin(), wires[i].end(), [](char c) { return std::isspace(c); }), wires[i].end());
        
            Wire *w = new Wire();
            w->label = wires[i];
            w->pre = NULL;
            N_W[wires[i]] = w;
            input_wire.push_back(w);
        }
    }
    else if (line.find("output") != string::npos)
    {
        pos = line.find("output");
        substring = line.substr(pos + 7);
        vector<string> wires = dis(substring, ",");
        for (int i = 0; i < wires.size(); i++)
        {
            wires[i].erase(remove_if(wires[i].begin(), wires[i].end(), ::isspace), wires[i].end());
            Wire *w = new Wire();
            w->label = wires[i];
            w->OUT = true;
            N_W[wires[i]] = w;
            OUT_name.push_back(wires[i]);
        }
    }
    
}


int compare_number(string input)
{
    string substring = input.substr(1);
    return stoi(substring);
}

void store_gate(string line)
{
    int pos = 0;
    string substring = "";

    if (line.find("INVX1") != string::npos)
    {
        Gate *n = new Gate();
        Wire *w;
        pos = line.find("INVX1");
        substring = line.substr(pos + 5);
        pos = substring.find("(");
        substring = substring.substr(0, pos);
        substring.erase(remove_if(substring.begin(), substring.end(), ::isspace), substring.end());
        n->label = substring;
        n->gate_type = 0;

        pos = line.find(".ZN(");
        substring = line.substr(pos + 4);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->ZN = w; 
        w->pre = n;

        pos = line.find(".I(");
        substring = line.substr(pos + 3);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->A1 = w; 
        w->next.push_back(n); 


        totalgate.push_back(n);
    }
    
    else if (line.find("NANDX1") != string::npos)
    {
        Gate *n = new Gate();
        Wire *w;
        pos = line.find("NANDX1");
        substring = line.substr(pos + 6);
        pos = substring.find("(");
        substring = substring.substr(0, pos);
        substring.erase(remove_if(substring.begin(), substring.end(), ::isspace), substring.end());
        n->label = substring;
        n->gate_type = 2;

        pos = line.find(".ZN(");
        substring = line.substr(pos + 4);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->ZN = w;
        w->pre = n;
        
        
        pos = line.find(".A1(");
        substring = line.substr(pos + 4);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->A1 = w;
        w->next.push_back(n);

        pos = line.find(".A2(");
        substring = line.substr(pos + 4);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->A2 = w;
        w->next.push_back(n);


        totalgate.push_back(n);
    }
    
    else if (line.find("NOR2X1") != string::npos)
    {
        Gate *n = new Gate();
        Wire *w;
        pos = line.find("NOR2X1");
        substring = line.substr(pos + 6);
        pos = substring.find("(");
        substring = substring.substr(0, pos);
        substring.erase(remove_if(substring.begin(), substring.end(), ::isspace), substring.end());
        n->label = substring;
        n->gate_type = 1;

        
        pos = line.find(".ZN(");
        substring = line.substr(pos + 4);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->ZN = w;
        w->pre = n;
        totalgate.push_back(n);
        
        
        pos = line.find(".A1(");
        substring = line.substr(pos + 4);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->A1 = w;
        w->next.push_back(n);


        pos = line.find(".A2(");
        substring = line.substr(pos + 4);
        pos = substring.find(")");
        substring = substring.substr(0, pos);
        w = N_W[substring];
        n->A2 = w;
        w->next.push_back(n);


        
    }
    
}

void store(ifstream &input){
  int pos = 0;
  bool isCommentBlock = false;
  string line;
  while (getline(input, line)){
    // Remove ';' from line
    line.erase(remove(line.begin(), line.end(), ';'), line.end());


    if (line == "endmodule")
    {
        break;
    }


    if (!isCommentBlock)
    {
        size_t commentPos = line.find("//");
        if (commentPos != string::npos)
        {
            line = line.substr(0, commentPos);
        }
        else
        {
            while (true)
            {
                size_t blockStart = line.find("/*");
                if (blockStart == string::npos)
                {
                    break;
                }
                size_t blockEnd = line.find("*/", blockStart);
                if (blockEnd == string::npos)
                {
                    isCommentBlock = true;
                    line.erase(blockStart);
                    break;
                }
                line.erase(blockStart, blockEnd - blockStart + 2);
            }
        }
    }
    else
    {
        size_t blockEnd = line.find("*/");
        if (blockEnd != string::npos)
        {
            isCommentBlock = false;
            line.erase(0, blockEnd + 2);
        }
        else
        {
            continue;
        }
    }

    if (line.empty())
    {
        continue;
    }
  
    if (line.find("input") != string::npos || line.find("output") != string::npos || line.find("wire") != string::npos){
      store_wire(line);
    }
    else if (line.find("INVX1") != string::npos || line.find("NOR2X1") != string::npos || line.find("NANDX1") != string::npos){
      store_gate(line);
    }
  }
}




bool loading_compare(Gate *a, Gate *b)
{
    if (a->o_cap > b->o_cap)
    {
        return true;
    }
    else if (a->o_cap == b->o_cap && compare_number(a->label) < compare_number(b->label))
    {
        return true;
    }
    return false;
}


void output_capacitance()
{

    for (const auto &g : totalgate)
    {
        if (g->ZN->OUT)
        {
            // PO
            g->o_cap = 0.03;
        }

        for (const auto &n : g->ZN->next) 
        {
            if (n->gate_type == 0)
            {
                // INVX1
                g->o_cap += INVX1_I_capacitance;
            }
            else if (n->gate_type == 1)
            {
                // NOR2X1
                if (n->A1 == g->ZN)
                {
                    g->o_cap += NOR2X1_A1_capacitance;
                }
                else if (n->A2 == g->ZN)
                {
                    g->o_cap += NOR2X1_A2_capacitance;
                }
                else
                {
                    cerr << "ERROR" << endl;
                }
            }
            else if (n->gate_type == 2)
            {
                // NANDX1
                if (n->A1 == g->ZN)
                {
                    g->o_cap += NANDX1_A1_capacitance;
                }
                else if (n->A2 == g->ZN)
                {
                    g->o_cap += NANDX1_A2_capacitance;
                }
                else
                {
                    cerr << "ERROR" << endl;
                }
            }
            else
            {
                cerr << "ERROR" << endl;
            }
        }
    }

    sort(totalgate.begin(), totalgate.end(), loading_compare);
}


double interpolation(const vector<vector<double>> M, double o_cap, double in_tran)
{

    int a, b, c, d;
    double front, end, temp, output;
    
    if (in_tran < all_in_trans[0]){
        a = 0;
        b = 1;
    }
    else if (in_tran > all_in_trans[6]){
        a = 5;
        b = 6;
    }
    else{
        for (int i = 1; i < 7; i++){
            if (in_tran == all_in_trans[i]){
                a = i;
                b = i;
                break;
            }
            
            else if (in_tran == all_in_trans[i - 1]){
                a = i - 1;
                b = i - 1;
                break;
            }
        
            else if (in_tran > all_in_trans[i - 1] && in_tran < all_in_trans[i]){
                a = i - 1;
                b = i;
                break;
            }
        }
    }
    
    
    if (o_cap > all_cap[6]){
        c = 5;
        d = 6;
    }
    
    else if (o_cap < all_cap[0]){
        c = 0;
        d = 1;
    }
    
    else{
        for (int i = 1; i < 7; i++){
            if (o_cap == all_cap[i]){
                c = i;
                d = i;
                break;
            }
            else if (o_cap == all_cap[i - 1]){
                c = i - 1;
                d = i - 1;
                break;
            }
            
            else if (o_cap > all_cap[i - 1] && o_cap < all_cap[i]){
                c = i - 1;
                d = i;
                break;
            }
        }
    }




    
    
    if (o_cap < all_cap[0]){
        temp = (all_cap[c] - o_cap) / (all_cap[d] - all_cap[c]);
        front = M[a][c] - ((M[a][d] - M[a][c]) * temp);
        end = M[b][c] - ((M[b][d] - M[b][c]) * temp);
    }
    else if (o_cap > all_cap[6]){
        temp = (o_cap - all_cap[d]) / (all_cap[d] - all_cap[c]);
        front = M[a][d] + ((M[a][d] - M[a][c]) * temp);
        end = M[b][d] + ((M[b][d] - M[b][c]) * temp);
    }
    else{
        temp = (o_cap - all_cap[c]) / (all_cap[d] - all_cap[c]);
        front = M[a][c] + ((M[a][d] - M[a][c]) * temp);
        end = M[b][c] + ((M[b][d] - M[b][c]) * temp);
    }
    
    
    

    if (in_tran < all_in_trans[0]){
        output = front - (end - front) / (all_in_trans[b] - all_in_trans[a]) * (all_in_trans[a] - in_tran);
    }
    else if (in_tran > all_in_trans[6]){
        output = end + (end - front) / (all_in_trans[b] - all_in_trans[a]) * (in_tran - all_in_trans[b]);
    }
    else{
        output = front + (end - front) / (all_in_trans[b] - all_in_trans[a]) * (in_tran - all_in_trans[a]);
    }


    return output;
}


bool delay_compare(Gate *a, Gate *b)
{
    if (a->delay > b->delay)
    {
        return true;
    }
    else if (a->delay == b->delay && compare_number(a->label) < compare_number(b->label))
    {
        return true;
    }
    return false;
}


void choose_the_worst()
{
    queue<Gate *> q;

    
    for(int i = 0; i < input_wire.size(); i++){
      Wire *w = input_wire[i];
      for (const auto &g : w->next){
        q.push(g);
      }

    }
    
    while (!q.empty())
    {
        Gate *current_gate = q.front();
        q.pop();
        
        if (current_gate->total_delay != 0)
        {
            continue;
        }
        
        // INVX1
        if (current_gate->gate_type == 0)
        {
            double transition_time_temp;
            double a0, a1;
            
            if(current_gate->A1->pre == NULL){
              transition_time_temp = 0;
              current_gate->total_delay = 0;
            }
            
            else{
            
              if(current_gate->A1->pre->delay == -1){
                continue;
              }
              
              else{
                transition_time_temp = current_gate->A1->pre->trans;
                current_gate->total_delay = current_gate->A1->pre->total_delay;
                current_gate->path = current_gate->A1->pre->path;
              }
              
            }
            
            a0 = interpolation(INVX1_cell_fall, current_gate->o_cap, transition_time_temp);
            a1 = interpolation(INVX1_cell_rise, current_gate->o_cap, transition_time_temp);

            if(a0 > a1){
              current_gate->delay = a0;
              current_gate->trans = interpolation(INVX1_fall_transition, current_gate->o_cap, transition_time_temp);
              current_gate->ZN->value = 0;
            }
            else{
              current_gate->delay = a1;
              current_gate->trans = interpolation(INVX1_rise_transition, current_gate->o_cap, transition_time_temp);
              current_gate->ZN->value = 1;
            }   
            
            if(current_gate->A1->pre == NULL){
              current_gate->total_delay = current_gate->total_delay + current_gate->delay;
            }
            else{
              current_gate->total_delay = current_gate->total_delay + current_gate->delay +0.005;
            }
            
            current_gate->path.push_back(current_gate->A1);
            
            for (const auto &g : current_gate->ZN->next){
              q.push(g);
            }
        }
        
        //NOR
        else if (current_gate->gate_type == 1){
        
          double transition_time_temp;
          double a0, a1;
    
          if(current_gate->A1->pre == NULL &&  current_gate->A2->pre == NULL){
              transition_time_temp = 0;
              current_gate->total_delay = 0;
          }
          
          
          
          else if(current_gate->A1->pre == NULL && current_gate->A2->pre != NULL){
          
            if(current_gate->A2->pre->delay == -1){
                continue;
            }
            else{
              transition_time_temp = current_gate->A2->pre->trans;
              current_gate->total_delay = current_gate->A2->pre->total_delay;
              current_gate->path = current_gate->A2->pre->path;
              current_gate->path.push_back(current_gate->A2);
            }
            
          }
          
          else if(current_gate->A2->pre == NULL && current_gate->A1->pre != NULL){
            if(current_gate->A1->pre->delay == -1){
                continue;
            }
            else{
              transition_time_temp = current_gate->A1->pre->trans;
              current_gate->total_delay = current_gate->A1->pre->total_delay;
              current_gate->path = current_gate->A1->pre->path;
              current_gate->path.push_back(current_gate->A1);
            }
          }
          // 前面皆有接線
          else if(current_gate->A2->pre != NULL && current_gate->A1->pre != NULL){
            
            if(current_gate->A1->pre->delay == -1 || current_gate->A2->pre->delay == -1){
                continue;
            }
            
            else if (current_gate->A1->pre->delay != -1 && current_gate->A2->pre->delay != -1){
            
              if(current_gate->A1->pre->total_delay > current_gate->A2->pre->total_delay){
              
                transition_time_temp = current_gate->A1->pre->trans;
                current_gate->total_delay = current_gate->A1->pre->total_delay;
                current_gate->path = current_gate->A1->pre->path;
                current_gate->path.push_back(current_gate->A1);
              }
              else{
                transition_time_temp = current_gate->A2->pre->trans;
                current_gate->total_delay = current_gate->A2->pre->total_delay;
                current_gate->path = current_gate->A2->pre->path;
                current_gate->path.push_back(current_gate->A2);
              }
            }
            
          }
           
          a0 = interpolation(NOR2X1_cell_fall, current_gate->o_cap, transition_time_temp);
          a1 = interpolation(NOR2X1_cell_rise, current_gate->o_cap, transition_time_temp);
          
          if(a0 > a1){
            current_gate->delay = a0;
            current_gate->trans = interpolation(NOR2X1_fall_transition, current_gate->o_cap, transition_time_temp);
            current_gate->ZN->value = 0;
            
          }
          else{
            current_gate->delay = a1;
            current_gate->trans = interpolation(NOR2X1_rise_transition, current_gate->o_cap, transition_time_temp);
            current_gate->ZN->value = 1;
          }  
          
          if(current_gate->A1->pre == NULL &&  current_gate->A2->pre == NULL){
              current_gate->total_delay = current_gate->total_delay + current_gate->delay;
          }
          else{
            current_gate->total_delay = current_gate->total_delay + current_gate->delay + 0.005;
          }
          
          
          
          
          for (const auto &g : current_gate->ZN->next){
            q.push(g);
          }
          
          
        }
        
        // NAND
        
        else if (current_gate->gate_type == 2){
        
          double transition_time_temp;
          double a0, a1;
          
    
          if(current_gate->A1->pre == NULL &&  current_gate->A2->pre == NULL){
              transition_time_temp = 0;
              current_gate->total_delay = 0;
              current_gate->path.push_back(current_gate->A1);
          }
            
          else if(current_gate->A1->pre == NULL && current_gate->A2->pre != NULL){
          
            if(current_gate->A2->pre->delay == -1){
                continue;
            }
            else{
              transition_time_temp = current_gate->A2->pre->trans;
              current_gate->total_delay = current_gate->A2->pre->total_delay;
              current_gate->path = current_gate->A2->pre->path;
              current_gate->path.push_back(current_gate->A2);
            }
            
          }
          
          else if(current_gate->A2->pre == NULL && current_gate->A1->pre != NULL){
            if(current_gate->A1->pre->delay == -1){
                continue;
            }
            else{
              transition_time_temp = current_gate->A1->pre->trans;
              current_gate->total_delay = current_gate->A1->pre->total_delay;
              current_gate->path = current_gate->A1->pre->path;
              current_gate->path.push_back(current_gate->A1);
            }
          }
          // 前面皆有接線
          else {
            
            if(current_gate->A1->pre->delay == -1 || current_gate->A2->pre->delay == -1){
                continue;
            }
            
            else{
              
              if(current_gate->A1->pre->total_delay > current_gate->A2->pre->total_delay){
                transition_time_temp = current_gate->A1->pre->trans;
                current_gate->total_delay = current_gate->A1->pre->total_delay;
                current_gate->path = current_gate->A1->pre->path;
                current_gate->path.push_back(current_gate->A1);
              }
              else{
                transition_time_temp = current_gate->A2->pre->trans;
                current_gate->total_delay = current_gate->A2->pre->total_delay;
                current_gate->path = current_gate->A2->pre->path;
                current_gate->path.push_back(current_gate->A2);
              }
            }
            
          }
          
          
          a0 = interpolation(NANDX1_cell_fall, current_gate->o_cap, transition_time_temp);
          a1 = interpolation(NANDX1_cell_rise, current_gate->o_cap, transition_time_temp);
          
          if(a0 > a1){
            current_gate->delay = a0;
            current_gate->trans = interpolation(NANDX1_fall_transition, current_gate->o_cap, transition_time_temp);
            current_gate->ZN->value = 0;
            
          }
          else{
            current_gate->delay = a1;
            current_gate->trans = interpolation(NANDX1_rise_transition, current_gate->o_cap, transition_time_temp);
            current_gate->ZN->value = 1;
          }  
          
         if(current_gate->A1->pre == NULL &&  current_gate->A2->pre == NULL){
              current_gate->total_delay = current_gate->total_delay + current_gate->delay;
          }
          else{
            current_gate->total_delay = current_gate->total_delay + current_gate->delay + 0.005;
          }
          
          
          
          
          for (const auto &g : current_gate->ZN->next){
            q.push(g);
          }
          
      }
        

    }
    
    
    sort(totalgate.begin(), totalgate.end(), delay_compare);
    
}

int main(int argc, char* argv[]){
  int i;
  ifstream netlist(argv[1]);
  ifstream lib(argv[3]);
  
  string file_name = argv[1];
  int pos = file_name.find(".v");
  string name = file_name.substr(0, pos);
  
  ofstream outputFile1("312510188_" + name + "_load.txt");
  
    
  store(netlist);
  ///step 1
  output_capacitance();
  
  for(i = 0; i < totalgate.size(); i++){
    outputFile1 << totalgate[i]->label << " " << fixed << setprecision(6) << totalgate[i]->o_cap << endl;
  }  
  
  // Step 2 
  ofstream outputFile2("312510188_" + name + "_delay.txt");
  choose_the_worst();
  
  
  for(i = 0 ; i < totalgate.size(); i++){
    outputFile2 << totalgate[i]->label << " " << totalgate[i]->ZN->value << " " << fixed << setprecision(6) << totalgate[i]->delay << " " << fixed << setprecision(6) << 
    totalgate[i]->trans << endl;
  }
  
  
  
  // step3
  ofstream outputFile3("312510188_" + name + "_path.txt");
  string longest_output;
  string shortest_output;
  double longest_delay = 0;
  double shortest_delay = 9999999999999999;
  vector<Wire *> longest_path;
  vector<Wire *> shortest_path;

  for (const auto &N : OUT_name){
    if (N_W[N]->pre->total_delay > longest_delay){
      longest_output = N_W[N]->label;
      longest_delay = N_W[N]->pre->total_delay;
      longest_path = N_W[N]->pre->path;
    }
    if (N_W[N]->pre->total_delay < shortest_delay){
      shortest_output = N_W[N]->label;
      shortest_delay = N_W[N]->pre->total_delay;
      shortest_path = N_W[N]->pre->path;
    }
  }
  
  outputFile3 << "Longest delay = " << fixed << setprecision(6) << longest_delay << ", the path is: ";
  for (int i = 0; i < longest_path.size(); i++){
    outputFile3 << longest_path[i]->label << " -> ";
  }
  outputFile3 << longest_output << endl;
  outputFile3 << "Shortest delay = " << fixed << setprecision(6) << shortest_delay << ", the path is: ";
  for (int i = 0; i < shortest_path.size(); i++){
    outputFile3 << shortest_path[i]->label << " -> ";
  }
  outputFile3  << shortest_output << endl;
  outputFile3 << endl;
}






























