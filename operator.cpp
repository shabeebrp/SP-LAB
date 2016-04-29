#include<iostream>
#include<list>
#include<map>
#include<set>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
class production {
public:
	map<string, vector<list<string>>> prod_map;
	map<string, int> terminals;
	set<string> non_term;
	string start;
	void read(string filename) {
		ifstream fin(filename);
		string line;
		while (getline(fin, line)) {
			stringstream ss(line);
			string key, value, temp;
			list<string> l;
			ss >> key;
			ss >> value;
			if (prod_map.size() == 0)
				start = key;
			if (isupper(key[0])) {
				non_term.insert(key);
			}
			while (getline(ss, value, ' ')) {
				if (value.compare("\0") != 0) {
					if (!isupper(value[0])) {
						terminals[value] = terminals.size();
					}
					l.push_back(value);
				}
			}
			prod_map[key].push_back(l);
		}
		//cout << "\n"<<terminals.size();
	}
};
class precedence {
public:
	production p;
	string  table[20][20];
	map<string, set<string>> leading;
	map<string, set<string>> trailling;

	precedence(string filename) {
		p.read(filename);
		find_lead();
		find_trail();
		table_fill();
		print_lead_trail();
		print_table();
	}
	bool is_terminal(string str) {
		if (isupper(str[0]))
			return false;
		else
			return true;
	}
	void find_lead() {
		auto nonT = p.non_term;
		for (auto i = nonT.begin(); i != nonT.end(); i++) {
			leading[*i] = lead(*i);
		}
		for (auto i = nonT.begin(); i != nonT.end(); i++) {
			complete(*i,leading);
		}
		for (auto i = nonT.begin(); i != nonT.end(); i++) {
			remove_nonterm(*i,leading);
		}
	}
	void find_trail() {
		auto nonT = p.non_term;
		for (auto i = nonT.begin(); i != nonT.end(); i++) {
			trailling[*i] = trail(*i);
		}
		for (auto i = nonT.begin(); i != nonT.end(); i++) {
			complete(*i, trailling);
		}
		for (auto i = nonT.begin(); i != nonT.end(); i++) {
			remove_nonterm(*i, trailling);
		}
	}
	set<string> lead(string key) {
		set<string> out;
		auto prod_map = p.prod_map;
		for (auto i = prod_map[key].begin(); i != prod_map[key].end(); i++) {
			for (auto j = (*i).begin(); j != (*i).end(); j++) {
				auto before = j;
				if (before != (*i).begin())
					before--;
				if ((*i).size() == 1) {
					out.insert(*j);
					break;
				}
				else if (is_terminal(*j)) {
					if (j == (*i).begin()) {
						out.insert(*j);
						break;
					}
					else {
						out.insert(*j);
						out.insert(*before);
						break;
					}
				}
			}
		}
		return out;
	}
	set<string> trail(string key) {
		
		set<string> out;
		auto prod_map = p.prod_map;
		for (auto i = prod_map[key].begin(); i != prod_map[key].end(); i++) {
			for (auto j = (*i).rbegin(); j != (*i).rend(); j++) {
				auto after = j;
				if (after != (*i).rend())
					after++;
				if ((*i).size() == 1) {
					out.insert(*j);
					break;
				}
				else if (is_terminal(*j)) {
					if (j == (*i).rbegin()) {
						out.insert(*j);
						break;
					}
					else {
						out.insert(*j);
						out.insert(*after);
						break;
					}
				}
			}
		}
		return out;
	}
	void complete(string key,map<string,set<string>>& m) {
		for (auto i = m.begin(); i != m.end(); i++) {
			for (auto j = (i->second).begin(); j != (i->second).end(); j++) {
				if (*j == key)
					m[i->first].insert(m[key].begin(), m[key].end());
			}
		}
	}
	void remove_nonterm(string key, map<string, set<string>>& m) {
		for (auto i = m.begin(); i != m.end();i++) {
			i->second.erase(key);
		}
	}
	void table_fill() {
		p.terminals["$"] = p.terminals.size();
		auto prod_map = p.prod_map;
		for (auto i = prod_map.begin(); i != prod_map.end(); i++) {
			for (auto v = (i->second).begin(); v != (i->second).end(); v++) {
				analyze_prod(*v);
			}
		}
		for (auto i = leading[p.start].begin(); i != leading[p.start].end(); i++) {
			int m = p.terminals["$"];
			int n = p.terminals[*i];
			table[m][n] = "<";
		}
		for (auto i = leading[p.start].begin(); i != leading[p.start].end(); i++) {
			int n = p.terminals["$"];
			int m = p.terminals[*i];
			table[m][n] = ">";
		}
	}
	void analyze_prod(list<string> rhs) {
		if (rhs.size() < 2)
			return;
		for (auto s = rhs.begin(); s != rhs.end(); s++) {
			auto before = s;
			auto after = s;
			after++;
			if (before != rhs.begin())
				before--;
			if (after == rhs.end())
				after--;
			if (is_terminal(*s)) {
				if (!is_terminal(*after)) {
					int m = p.terminals[*s];
					for (auto i = leading[*after].begin(); i != leading[*after].end(); i++) {
						int n = p.terminals[*i];
						table[m][n] = "<";
					}
 				}
				else if (after != s) {
					int m = p.terminals[*s];
					int n = p.terminals[*after];
					table[m][n] = "=";
				}
				if (!is_terminal(*before)) {
					int n = p.terminals[*s];
					for (auto i = trailling[*before].begin(); i != trailling[*before].end(); i++) {
						int m = p.terminals[*i];
						table[m][n] = ">";
					}
				}

			}
			else {
				if (is_terminal(*before) && is_terminal(*after)) {
					int m = p.terminals[*before];
					int n = p.terminals[*after];
					table[m][n] = "=";
				}
			}
		}
	}
	void print_lead_trail() {
		cout << "LEADING"<< "\n------------------"<<endl;
		for (auto i = leading.begin(); i != leading.end(); i++) {
			cout << i->first<< "(" << i->second.size()<< ")\t:\t";
			for (auto j = (i->second).begin(); j != (i->second).end(); j++)
				cout << *j<<" ";
			cout << endl;
		}
		cout << "\nTrailling" << "\n------------------" << endl;
		for (auto i = trailling.begin(); i != trailling.end(); i++) {
			cout << i->first << "(" << i->second.size() << ")\t:\t";
			for (auto j = (i->second).begin(); j != (i->second).end(); j++)
				cout << *j<<" ";
			cout << endl;
		}
	}
	void print_table() {
		auto term = p.terminals;
		int size = term.size();
		cout <<"\n\n";
		for (auto i = term.begin(); i != term.end(); i++) {
			cout << "\t"<<i->first ;
		}
		cout << "\n\n";
		for (auto i = term.begin(); i != term.end(); i++) {
			cout << i->first;
			int m = term[i->first];
			for (auto j = term.begin(); j != term.end(); j++) {
				int n = term[j->first];
				cout << "\t"  << table[m][n] ;
			}
			cout << "\n\n";
		}
	}

};
int main() {
	precedence pr("operator.txt");
	return 0;
}
