#include <bits/stdc++.h>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;
string router_ip;
set <string> allOtherRouter;
set <string> neighbours;
int sendclock=0;
int sockfd; 
class routerInfo{
public:
	string nextHop;
	int cost;

};
map <string,routerInfo>routerTable;
void initRouter(string filename){
	ifstream topo_file(filename.c_str());
	ifstream topo_file2(filename.c_str());

	string s1,s2;
	int c;
	while(!topo_file.eof()){

		topo_file>>s1>>s2>>c;
		cout<<s1<<" "<<s2<<" "<<c<<endl;
		if(s1.compare(router_ip)){
			allOtherRouter.insert(s1);
		}
		if(s2.compare(router_ip)){
			allOtherRouter.insert(s2);
		}
		
		if(!s1.compare(router_ip)){
			neighbours.insert(s2);
			routerInfo r;
			r.nextHop=s2;
			r.cost=c;
			routerTable[s2]=r;

		}
		else if(!s2.compare(router_ip)){
			neighbours.insert(s1);
			routerInfo r;
			r.nextHop=s1;
			r.cost=c;
			routerTable[s1]=r;

		}
	}
	
	set<string>::iterator it;
	for (it = allOtherRouter.begin(); it != allOtherRouter.end(); ++it)
	{
    		const bool is_in = neighbours.find(*it) != neighbours.end();
		if(is_in==false){
			routerInfo r;
			r.nextHop="null";
			r.cost=1000000;
			routerTable[*it]=r;
		}	
	}


		
	
	
	cout<<"initial";

}
int makeintFromString(string a){

		stringstream s(a);
		int x;
		s>>x;
		return x;

}
void printRouterTable(){
	cout<<"Router table:"<<endl;
	for(map<string,routerInfo >::const_iterator it = routerTable.begin();it != routerTable.end(); ++it){
    		cout << it->first << " " << it->second.nextHop << " " << it->second.cost << "\n";


	}

}
string makeTableString(){	
	string t="rrtb";
	t=t+"#"+router_ip;
	for(map<string,routerInfo >::const_iterator it = routerTable.begin();it != routerTable.end(); ++it){
    		//std::cout << it->first << " " << it->second.nextHop << " " << it->second.cost << "\n";
		t=t+"#"+it->first+"*"+it->second.nextHop+"*"+to_string(it->second.cost);
		
	}
	return t;
}
void send(){
	string send=makeTableString();
	set<string>::iterator it;
	for (it = neighbours.begin(); it != neighbours.end(); ++it)
	{	
		
    	struct sockaddr_in router_address;
		router_address.sin_family = AF_INET;
		router_address.sin_port = htons(4747);
		router_address.sin_addr.s_addr = inet_addr((*it).c_str());
		sendto(sockfd, send.c_str(), 1024, 0, (struct sockaddr*) &router_address, sizeof(sockaddr_in));
		
	}	



}

vector<string> updateTable2(string rev){
	vector <string> tokens;
	istringstream iss(rev);
	string token;
	while(getline(iss,token,'#')){
		tokens.push_back(token);
	}
	return tokens;
}

void updateTable(string rev){
	vector <string> v = updateTable2(rev);
	if (v.size()>5)
	{
		cout<<"problem"<<endl;
	}
	else{
		vector <string> table[v.size()-2];
		for(int i=0;i<v.size();i++){
			if(i!=0){
				if(i==1){
					cout<<"the msg is sent from :"+v[i]<<endl;
				}
				else{
					istringstream iss(v[i]);
					string token;
					while(getline(iss,token,'*')){

						table[i-2].push_back(token);
					
					
					//cout <<"token : "<< token << "\n";
					}
				}
			}
		}
	routerInfo r;
	for (int i = 0; i < 3; ++i)
	{
		//int x=(routerTable[v[1]]).cost;// cost of source to this router
		auto it = routerTable.find(v[1]);
		int x = -1;//x->jar kach theke table ashe tar sathe distance 1-2
		if(it != routerTable.end()){
			x = (it -> second).cost;
		}
		int y=-1;
		y= makeintFromString(table[i][2]);//y=
		int z = -1;
		auto iter = routerTable.find(table[i][0]);
		if(iter != routerTable.end()){
			z = (iter -> second).cost;
		}
		//cout<<"values are : "<<x<<" "<<y<<" "<<z<<endl;
		if (y+x<z && v[1].compare(router_ip) && x!=-1 && y!=-1 && z!=-1)
		{
//			(routerTable[v[1]]).cost=x+y;
			(iter -> second).cost = x + y;
			(iter -> second).nextHop = (it -> second).nextHop;
		}
	}
	}

		 
}
void handle_command(string com,string rev){
	
	if(!com.compare("show")){
		printRouterTable();
	}
	else if(!com.compare("clk ")){
		sendclock++;
		send();
	
	}
	else if(!com.compare("rrtb")){
		//cout<<"command :  \n"<<rev<<"\n";
		updateTable(rev);
		printRouterTable();
		
	}
	else if (!com.compare("cost"))
	{
		cout<<"update cost"<<endl;
		cout<<"string : "<<rev<<endl;

	}

}

int main(int argc,char *argv[]){
	router_ip=argv[1];
	cout<<router_ip<<endl;
	initRouter(argv[2]);
	printRouterTable();

	
	int bind_flag;
	int bytes_received;
	socklen_t addrlen;
	char buffer[1024];
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	cout<<"size : neighbours "<<neighbours.size();
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(4747);
	server_address.sin_addr.s_addr = inet_addr(argv[1]);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind_flag = bind(sockfd, (struct sockaddr*) &server_address, sizeof(sockaddr_in));
	while(true){
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &client_address, &addrlen);
		//printf("[%s:%d]: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);
		if(bytes_received!=-1){
			string rev(buffer);
			string com = rev.substr(0,4);
			handle_command(com,rev);
			
		}
	}


return 0;
}
