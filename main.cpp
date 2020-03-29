// Single Database -> Single Table System

#include<bits/stdc++.h>
#include<fstream>
#include<sys/stat.h>
#include<sys/types.h>
#include<string>
#include<string.h>

# define BLOCK_SIZE 4
# define NODE_SIZE 4
# define DELIMITER ", "

using namespace std;
vector<string> printDBList(){
    cout<<"List of existing Databases : \n";
    fstream list_file;
    list_file.open("list.csv", ios::in|ios::out|ios::app);
    list_file.seekg(0, ios::beg);
    vector<string> dbs;
    while(list_file){
        string name;
        list_file>>name;
        dbs.push_back(name);
    }
    if(dbs.size() == 1){
        cout<<"Empty\n";
    }else{
        for(int i=0;i<dbs.size()-1;i++){
            cout<<i+1<<") "<<dbs[i]<<"\n";
        }
    }
    cout<<"\nEnter any character to proceed : ";
    char y;
    cin>>y;
    cin.ignore();
    return dbs;
}
bool searchDB(string name){
    fstream list_file;
    list_file.open("list.csv", ios::in|ios::out|ios::app);
    list_file.seekg(0, ios::beg);
    vector<string> dbs;
    while(list_file){
        string curr;
        list_file>>curr;
        dbs.push_back(curr);
    }
    list_file.close();
    if(dbs.size() == 1){
        return false;
    }else{
        for(int i=0;i<dbs.size()-1;i++){
            if(dbs[i] == name) return true;
        }
    }
    return false;
}
bool createDB(string name){
    const char* db_name = name.c_str();
    fstream list_file;
    list_file.open("list.csv", ios::in|ios::out|ios::app);
    list_file.seekg(0, ios::beg);
    if(mkdir(db_name, 0777) == -1){
        list_file.close();
        return false;
    }else{
        list_file<<db_name<<"\n";
        list_file.close();
        return true;
    }
}
void removeDB(string name){
    const char* db_name = name.c_str();
    fstream list_file;
    list_file.open("list.csv", ios::in|ios::out|ios::app);
    if(remove(db_name)){
        cout<<strerror( errno )<<"\n";
        list_file.close();
    }else{
        list_file.seekg(0, ios::beg);
        fstream temp;
        temp.open("temp.csv", ios::in|ios::out|ios::trunc);
        while(list_file){
            string curr;
            list_file>>curr;
            if(curr != name && curr.length() >= 1){
                temp<<curr<<"\n";
            }
        }
        list_file.close();
        temp.close();
        remove("list.csv");
        rename("temp.csv", "list.csv");
        cout<<"database removed\n";
    }
}

void createInfo(string name){
    fstream info;
    info.open(name + "/info.csv", ios::in|ios::out|ios::trunc);
    info<<"empty"<<DELIMITER<<"1"<<"\n";
    info<<"numAtts"<<DELIMITER<<"0"<<"\n";
    info<<"size"<<DELIMITER<<"0"<<"\n";
    info<<"index"<<DELIMITER<<"0"<<"\n";
    info<<"root"<<DELIMITER<<"null"<<"\n";
    info.close();
}
void createIndex(string db_name){
    string name = db_name + "/index";
    const char* index = name.c_str();
    if(mkdir(index, 0777) == -1){
        cout<<"error in creating index\n";
    }else{
        cout<<"default 0 based primary index of integer data type is created for clustered indexing\n";
    }
    return;
}
vector<string> getRow(string row){
    vector<string> cols;
    string val = "";
    for(int i=0;i<row.length();i++){
        if(row[i] == ','){
            cols.push_back(val);
            val = "";
            i++; // for white space
        }else{
            val += row[i];
        }
    }
    cols.push_back(val);
    return cols;
}
vector<vector<string>> getFile(string base, string file_name){
    vector<vector<string>> file_vector;
    fstream file;
    file.open(base + "/" + file_name + ".csv", ios::in|ios::out|ios::app);
    file.seekg(0, ios::beg);
    while(file){
        string curr;
        getline(file,curr);
        if(curr.length() > 0) file_vector.push_back(getRow(curr));
    }
    file.close();
    return file_vector;
}
void setFile(string base, string file_name, vector<vector<string>> file_vector){
    fstream file;
    file.open(base + "/" + file_name + ".csv", ios::in|ios::out|ios::trunc);
    for(int i=0;i<file_vector.size();i++){
        file<<file_vector[i][0];
        for(int j=1;j<file_vector[i].size();j++){
            file<<DELIMITER<<file_vector[i][j];
        }
        file<<"\n";
    }
    file.close();
}
void createSchema(string db_name){
    vector<vector<string>> info_vector = getFile(db_name, "info");
    if(info_vector[0][1] == "0"){
        cout<<"table already exists\n";
        return;
    }
    info_vector[0][1] = "0";
    
    // Create Schema
    fstream schema;
    schema.open(db_name + "/schema.csv", ios::in|ios::out|ios::trunc);
    cout<<"enter no. of attributes (excluding default primary index) : ";
    int numAtts;
    cin>>numAtts;
    info_vector[1][1] = to_string(numAtts);
    createIndex(db_name);
    schema<<"index"<<DELIMITER<<"int"<<"\n";
    cout<<"enter schema : ( <attribute_name> <data_type> )\n";
    for(int i=0;i<numAtts;i++){
        string att, dtype;
        cin>>att>>dtype;
        schema<<att<<DELIMITER<<dtype<<"\n";
    }
    cin.ignore();
    schema.close();
    info_vector[4][1] = "null";
    setFile(db_name, "info", info_vector);
}
//-----------------------------------------------------------------------------------------------------------------------------------------

vector<string> getPred(string db_name, string root, vector<vector<string>> root_vector){
    int n = root_vector.size();
    if(root_vector[n-1][4] == "null") return root_vector[n-1];
    string right_child = root_vector[n-1][4];
    vector<vector<string>> rc_vector = getFile(db_name + "/index", right_child);
    return getPred(db_name, right_child, rc_vector);
}
vector<string> getSucc(string db_name, string root, vector<vector<string>> root_vector){
    int n = root_vector.size();
    if(root_vector[1][0] == "null") return root_vector[1];
    string left_child = root_vector[1][0];
    vector<vector<string>> lc_vector = getFile(db_name + "/index", left_child);
    return getPred(db_name, left_child, lc_vector);
}
void removeUtil(string db_name, string index, string& root, bool flag = false){
    vector<vector<string>> root_vector = getFile(db_name + "/index", root);
    int idx = stoi(index);
    int i;
    for(i=1;i<root_vector.size();i++){
        int left = stoi(root_vector[i][1]);
        int right = left + NODE_SIZE - 1;
        if(idx >= left && idx <= right){
            string block_name = root_vector[i][2];
            int count = stoi(root_vector[i][3]);
            if(flag){
                // for pred and succ
                root_vector.erase(root_vector.begin() + i);
                setFile(db_name + "/index", root, root_vector);
                cout<<"deletion successful\n";
                return;
            }
            else if(count == 1){
                // delete the block
                if(root_vector[0][0] == "true"){
                    // simply delete the block file
                    root_vector.erase(root_vector.begin() + i);
                    string temp = db_name + "/" + block_name + ".csv";
                    const char* block = temp.c_str();
                    remove(block);
                    setFile(db_name + "/index", root, root_vector);
                    cout<<"deletion successful\n";
                    return;
                }else{
                    // delete the block file and merge if necessary
                    string left_child = root_vector[i][0];
                    vector<vector<string>> lc_vector = getFile(db_name + "/index", left_child);
                    if(lc_vector.size() >= NODE_SIZE/2 + 1){
                        // replace current i with its predecessor and delete the predecessor
                        vector<string> pred = getPred(db_name, left_child, lc_vector);
                        removeUtil(db_name, pred[1], left_child, true);
                        pred[0] = root_vector[i][0];
                        pred[4] = root_vector[i][4];
                        root_vector[i] = pred;
                        string temp = db_name + "/" + block_name + ".csv";
                        const char* block = temp.c_str();
                        remove(block);
                        setFile(db_name + "/index", root, root_vector);
                        return;
                    }
                    string right_child = root_vector[i][4];
                    vector<vector<string>> rc_vector = getFile(db_name + "/index", right_child);
                    if(rc_vector.size() >= NODE_SIZE/2 + 1){
                        // replace current i with its successor and delete the successor
                        vector<string> succ = getSucc(db_name, right_child, rc_vector);
                        removeUtil(db_name, succ[1], right_child, true);
                        succ[0] = root_vector[i][0];
                        succ[4] = root_vector[i][4];
                        root_vector[i] = succ;
                        string temp = db_name + "/" + block_name + ".csv";
                        const char* block = temp.c_str();
                        remove(block);
                        setFile(db_name + "/index", root, root_vector);
                        return;
                    }
                    // merge left_child and right_child
                    vector<string> target = root_vector[i];
                    target[0] = lc_vector[lc_vector.size()-1][4];
                    target[4] = rc_vector[1][0];
                    
                    if(root_vector.size() == 2){
                        string old_root = db_name + "/index/" + root + ".csv";
                        const char* r = old_root.c_str();
                        remove(r);
                        root = left_child;
                    }else{
                        if(i == 1){
                            root_vector[i+1][0] = left_child;
                        }else if(i == root_vector.size() - 1){
                            root_vector[i-1][4] = left_child;
                        }else{
                            root_vector[i+1][0] = left_child;
                            root_vector[i-1][4] = left_child;
                        }
                        root_vector.erase(root_vector.begin() + i);
                        setFile(db_name + "/index", root, root_vector);
                    }
                    
                    
                    lc_vector.push_back(target);
                    for(int j=1;j<rc_vector.size();j++){
                        lc_vector.push_back(rc_vector[j]);
                    }
                    
                    string temp = db_name + "/index/" + right_child + ".csv";
                    const char* rc = temp.c_str();
                    remove(rc);
                    setFile(db_name + "/index", left_child, lc_vector);
                    removeUtil(db_name, index, left_child);
                    return;
                }
            }else{
                vector<vector<string>> block_vector = getFile(db_name, block_name);
                for(int j=0;j<block_vector.size();j++){
                    if(stoi(block_vector[j][0]) == idx){
                        // delete the row
                        block_vector.erase(block_vector.begin() + j);
                        break;
                    }
                }
                count--;
                root_vector[i][3] = to_string(count);
                setFile(db_name, block_name, block_vector);
                setFile(db_name + "/index", root, root_vector);
                cout<<"deletion successful\n";
                return;
            }
        }else if(idx < left){
            break;
        }
    }
    string child;
    if(i == root_vector.size()){
        child = root_vector[i-1][4];
    }else{
        child = root_vector[i][0];
    }
    vector<vector<string>> child_vector = getFile(db_name + "/index", child);
    if(child_vector.size() >= NODE_SIZE/2 + 1){
        removeUtil(db_name, index, child);
    }else{
        if(i == 1){
            string right_sibling = root_vector[i][4];

            vector<vector<string>> rs_vector = getFile(db_name + "/index", right_sibling);
            if(rs_vector.size() >= NODE_SIZE/2 + 1){
                vector<string> x = root_vector[i];
                vector<string> y = rs_vector[1];
                x[0] = child_vector[child_vector.size()-1][4];
                x[4] = y[0];
                y[0] = child;
                y[4] = right_sibling;
                root_vector.erase(root_vector.begin() + i);
                child_vector.push_back(x);
                rs_vector.erase(rs_vector.begin() + 1);
                root_vector.emplace(root_vector.begin() + i, y);
                setFile(db_name + "/index", child, child_vector);
                setFile(db_name + "/index", right_sibling, rs_vector);
                setFile(db_name + "/index", root, root_vector);
                removeUtil(db_name, index, child);
                return;
            }
            vector<string> median = root_vector[i];
            median[0] = child_vector[child_vector.size()-1][4];
            median[4] = rs_vector[1][0];
            if(root_vector.size() == 2){
                string old_root = db_name + "/index/" + root + ".csv";
                const char* r = old_root.c_str();
                remove(r);
                root = child;
            }else{
                root_vector[i+1][0] = child;
                root_vector.erase(root_vector.begin() + i);
                setFile(db_name + "/index", root, root_vector);
            }
            
            child_vector.push_back(median);
            for(int j=1;j<rs_vector.size();j++){
                child_vector.push_back(rs_vector[j]);
            }
            string temp = db_name + "/index/" + right_sibling + ".csv";
            const char* rs = temp.c_str();
            remove(rs);
            setFile(db_name + "/index", child, child_vector);
            removeUtil(db_name, index, child);
            return;
        }else if(i == root_vector.size()){
            string left_sibling = root_vector[i-1][0];

            vector<vector<string>> ls_vector = getFile(db_name + "/index", left_sibling);
            if(ls_vector.size() >= NODE_SIZE/2 + 1){
                vector<string> x = root_vector[i - 1];
                vector<string> y = ls_vector[ls_vector.size() - 1];
                x[0] = y[4];
                x[4] = child_vector[1][0];
                y[0] = left_sibling;
                y[4] = child;
                root_vector.erase(root_vector.begin() + i - 1);
                child_vector.emplace(child_vector.begin() + 1, x);
                ls_vector.erase(ls_vector.begin() + ls_vector.size() - 1);
                root_vector.emplace(root_vector.begin() + i - 1, y);
                setFile(db_name + "/index", child, child_vector);
                setFile(db_name + "/index", left_sibling, ls_vector);
                setFile(db_name + "/index", root, root_vector);
                removeUtil(db_name, index, child);
                return;
            }
            vector<string> median = root_vector[i - 1];
            median[0] = ls_vector[ls_vector.size()-1][4];
            median[4] = child_vector[1][0];
            if(root_vector.size() == 2){
                string old_root = db_name + "/index/" + root + ".csv";
                const char* r = old_root.c_str();
                remove(r);
                root = child;
            }else{
                root_vector[i-1][4] = child;
                root_vector.erase(root_vector.begin() + i - 1);
                setFile(db_name + "/index", root, root_vector);
            }
            
            child_vector.push_back(median);
            for(int j=1;j<ls_vector.size();j++){
                child_vector.push_back(ls_vector[j]);
            }
            string temp = db_name + "/index/" + left_sibling + ".csv";
            const char* ls = temp.c_str();
            remove(ls);
            setFile(db_name + "/index", child, child_vector);
            removeUtil(db_name, index, child);
            return;
        }else{
            string left_sibling = root_vector[i-1][0];
            string right_sibling = root_vector[i][4];

            vector<vector<string>> ls_vector = getFile(db_name + "/index", left_sibling);
            if(ls_vector.size() >= NODE_SIZE/2 + 1){
                vector<string> x = root_vector[i - 1];
                vector<string> y = ls_vector[ls_vector.size() - 1];
                x[0] = y[4];
                x[4] = child_vector[1][0];
                y[0] = left_sibling;
                y[4] = child;
                root_vector.erase(root_vector.begin() + i - 1);
                child_vector.emplace(child_vector.begin() + 1, x);
                ls_vector.erase(ls_vector.begin() + ls_vector.size() - 1);
                root_vector.emplace(root_vector.begin() + i - 1, y);
                setFile(db_name + "/index", child, child_vector);
                setFile(db_name + "/index", left_sibling, ls_vector);
                setFile(db_name + "/index", root, root_vector);
                removeUtil(db_name, index, child);
                return;
            }

            vector<vector<string>> rs_vector = getFile(db_name + "/index", right_sibling);
            if(rs_vector.size() >= NODE_SIZE/2 + 1){
                vector<string> x = root_vector[i];
                vector<string> y = rs_vector[1];
                x[0] = child_vector[child_vector.size()-1][4];
                x[4] = y[0];
                y[0] = child;
                y[4] = right_sibling;
                root_vector.erase(root_vector.begin() + i);
                child_vector.push_back(x);
                rs_vector.erase(rs_vector.begin() + 1);
                root_vector.emplace(root_vector.begin() + i, y);
                setFile(db_name + "/index", child, child_vector);
                setFile(db_name + "/index", right_sibling, rs_vector);
                setFile(db_name + "/index", root, root_vector);
                removeUtil(db_name, index, child);
                return;
            }

            // merge
            vector<string> median = root_vector[i];
            median[0] = child_vector[child_vector.size()-1][4];
            median[4] = rs_vector[1][0];
            if(root_vector.size() == 2){
                string old_root = db_name + "/index/" + root + ".csv";
                const char* r = old_root.c_str();
                remove(r);
                root = child;
            }else{
                if(i == root_vector.size() - 1){
                    root_vector[i-1][4] = child;
                }else{
                    root_vector[i+1][0] = child;
                    root_vector[i-1][4] = child;
                }
                root_vector.erase(root_vector.begin() + i);
                setFile(db_name + "/index", root, root_vector);
            }
            
            child_vector.push_back(median);
            for(int j=1;j<rs_vector.size();j++){
                child_vector.push_back(rs_vector[j]);
            }
            string temp = db_name + "/index/" + right_sibling + ".csv";
            const char* rs = temp.c_str();
            remove(rs);
            setFile(db_name + "/index", child, child_vector);
            removeUtil(db_name, index, child);
            return;
        }
    }
}
void removeIndex(string db_name, string index){
    vector<vector<string>> info_vector = getFile(db_name, "info");
    string root = info_vector[4][1];
    removeUtil(db_name, index, root);
    int size = stoi(info_vector[2][1]);
    size--;
    info_vector[2][1] = to_string(size);
    info_vector[4][1] = root;
    setFile(db_name, "info", info_vector);
}

//-------------------------------------------------------------------------------------------------------------------------------------------
bool searchUtil(string db_name, string index, string root){
    if(root == "null") return false;
    vector<vector<string>> root_vector = getFile(db_name + "/index", root);
    int idx = stoi(index);
    int i;
    for(i=1;i<root_vector.size();i++){
        int left = stoi(root_vector[i][1]);
        int right = left + NODE_SIZE - 1;
        if(idx >= left && idx <= right){
            string block_name = root_vector[i][2];
            vector<vector<string>> block_vector = getFile(db_name, block_name);
            for(int j=0;j<block_vector.size();j++){
                if(stoi(block_vector[j][0]) == idx){
                    for(int k=0;k<block_vector[j].size();k++){
                        cout<<block_vector[j][k]<<" ";
                    }
                    cout<<"\n";
                    return true;
                }
            }
            return false;
        }else if(idx < left){
            break;
        }
    }
    string child;
    if(i == root_vector.size()){
        child = root_vector[i-1][4];
    }else{
        child = root_vector[i][0];
    }
    return searchUtil(db_name, index, child);
}
bool searchIndex(string db_name, string index){
    vector<vector<string>> info_vector = getFile(db_name, "info");
    string root = info_vector[4][1];
    return searchUtil(db_name, index, root);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void splitNode(string db_name, string& root, vector<vector<string>>& root_vector, string& child, vector<vector<string>>& child_vector, int i){
    int cn = child_vector.size();
    int mid = (cn-1)/2 + 1;
    
    vector<vector<string>> new_vector;
    new_vector.push_back(child_vector[0]);
    for(int j=mid + 1;j<cn;j++){
        new_vector.push_back(child_vector[j]);
    }
    vector<string> mid_node = child_vector[mid];
    for(int j=mid;j<cn;j++){
        child_vector.pop_back();
    }
    string new_file_name = to_string(rand());
    mid_node[0] = child;
    mid_node[4] = new_file_name;
    if(i < root_vector.size()){
        root_vector[i][0] = new_file_name;
    }
    if(root_vector.size() == 0){
        root_vector.push_back({"false"});
    }
    root_vector.emplace(root_vector.begin() + i, mid_node);

    setFile(db_name + "/index", child, child_vector);
    setFile(db_name + "/index", new_file_name, new_vector);
    setFile(db_name + "/index", root, root_vector);
}

void insertNonFullIndex(string db_name, string& root, vector<vector<string>>& root_vector, int idx, vector<string> row){
    int i;
    for(i=1;i<root_vector.size();i++){
        int left = stoi(root_vector[i][1]);
        int right = left + NODE_SIZE - 1;
        if(idx >= left && idx <= right){
            string block_name = root_vector[i][2];
            int count = stoi(root_vector[i][3]);
            if(count == BLOCK_SIZE){
                cout<<"id already exists\n";
                return;
            }else{
                vector<vector<string>> block_vector = getFile(db_name, block_name);
                bool done = false;
                for(int j=0;j<block_vector.size();j++){
                    if(stoi(block_vector[j][0]) == stoi(row[0])){
                        cout<<"id already exists\n";
                        return;
                    }else if(stoi(block_vector[j][0]) > stoi(row[0])){
                        auto temp = block_vector.emplace(block_vector.begin() + j, row);
                        done = true;
                        break;
                    }
                }
                if(done == false){
                    auto temp = block_vector.emplace(block_vector.end(), row);
                }
                count++;
                root_vector[i][3] = to_string(count);
                setFile(db_name, block_name, block_vector);
                setFile(db_name + "/index", root, root_vector);
                cout<<"insertion successful\n";
                return;
            }
        }else if(idx < left){
            break;
        }
    }
    if(root_vector[0][0] == "true"){
        // Create new block
        fstream block;
        string block_name = to_string(rand());
        block.open(db_name + "/" + block_name + ".csv", ios::in|ios::out|ios::trunc);

        // Update the block
        block<<row[0];
        for(int i=1;i<row.size();i++){
            block<<DELIMITER<<row[i];
        }
        block<<"\n";
        block.close();

        // Creating new node
        vector<string> node(5);
        node[0] = "null";
        node[1] = to_string(idx);
        node[2] = block_name;
        node[3] = "1";
        node[4] = "null";
        root_vector.emplace(root_vector.begin() + i, node);
        setFile(db_name + "/index", root, root_vector);
        cout<<"insertion successful\n";
        return;
    }else{
        string child;
        if(i == root_vector.size()){
            child = root_vector[i-1][4];
        }else{
            child = root_vector[i][0];
        }
        vector<vector<string>> child_vector = getFile(db_name + "/index", child);
        if(child_vector.size() == NODE_SIZE){
            // Split the child node
            splitNode(db_name, root, root_vector, child, child_vector, i);

            int left = stoi(root_vector[i][1]);
            int right = left + NODE_SIZE - 1;

            if(idx >= left && idx <= right){
                string block_name = root_vector[i][2];
                int count = stoi(root_vector[i][3]);
                if(count == BLOCK_SIZE){
                    cout<<"id already exists\n";
                    return;
                }else{
                    vector<vector<string>> block_vector = getFile(db_name, block_name);
                    bool done = false;
                    for(int j=0;j<block_vector.size();j++){
                        if(stoi(block_vector[j][0]) == stoi(row[0])){
                            cout<<"id already exists\n";
                            return;
                        }else if(stoi(block_vector[j][0]) > stoi(row[0])){
                            auto temp = block_vector.emplace(block_vector.begin() + j, row);
                            done = true;
                            break;
                        }
                    }
                    if(done == false){
                        auto temp = block_vector.emplace(block_vector.end(), row);
                    }
                    count++;
                    root_vector[i][3] = to_string(count);
                    setFile(db_name, block_name, block_vector);
                    setFile(db_name + "/index", root, root_vector);
                    cout<<"insertion successful\n";
                    return;
                }
            }else if(idx > right){
                child = root_vector[i][4];
                child_vector = getFile(db_name + "/index", child);
                insertNonFullIndex(db_name, child, child_vector, idx, row);
            }
        }else{
            insertNonFullIndex(db_name, child, child_vector, idx, row);
        }
    }
}

void insertIndex(string db_name, string& root, int idx, vector<string> row){
    if(root == "null"){
        fstream block;
        string block_name = to_string(rand());
        block.open(db_name + "/" + block_name + ".csv", ios::in|ios::out|ios::trunc);
        // Update the block
        block<<row[0];
        for(int i=1;i<row.size();i++){
            block<<DELIMITER<<row[i];
        }
        block<<"\n";
        block.close();
        root = to_string(rand());
        fstream index;
        index.open(db_name + "/index/" + root + ".csv", ios::in|ios::out|ios::trunc);
        // leaf index or not ?
        index<<"true"<<"\n"; 
        // less than pointer | range | block pointer | no. of rows in block | greater than pointer
        index<<"null"<<DELIMITER<<idx<<DELIMITER<<block_name<<DELIMITER<<1<<DELIMITER<<"null"<<"\n";
        index.close();
        cout<<"insertion successful\n";
    }else{
        vector<vector<string>> root_vector = getFile(db_name + "/index", root);
        if(root_vector.size() == NODE_SIZE){
            string new_root = to_string(rand());
            vector<vector<string>> new_root_vector;
            splitNode(db_name, new_root, new_root_vector, root, root_vector, 1);

            int left = stoi(new_root_vector[1][1]);
            int right = left + NODE_SIZE - 1;

            if(idx >= left && idx <= right){
                string block_name = new_root_vector[1][2];
                int count = stoi(new_root_vector[1][3]);
                if(count == BLOCK_SIZE){
                    cout<<"id already exists\n";
                    return;
                }else{
                    vector<vector<string>> block_vector = getFile(db_name, block_name);
                    bool done = false;
                    for(int j=0;j<block_vector.size();j++){
                        if(stoi(block_vector[j][0]) == stoi(row[0])){
                            cout<<"id already exists\n";
                            return;
                        }else if(stoi(block_vector[j][0]) > stoi(row[0])){
                            auto temp = block_vector.emplace(block_vector.begin() + j, row);
                            done = true;
                            break;
                        }
                    }
                    if(done == false){
                        auto temp = block_vector.emplace(block_vector.end(), row);
                    }
                    count++;
                    new_root_vector[1][3] = to_string(count);
                    setFile(db_name, block_name, block_vector);
                    setFile(db_name + "/index", new_root, new_root_vector);
                    cout<<"insertion successful\n";
                }
            }else if(idx > right){
                root = new_root_vector[1][4];
                root_vector = getFile(db_name + "/index", root);
                insertNonFullIndex(db_name, root, root_vector, idx, row);
            }
            root = new_root;
            return;
        }else{
            insertNonFullIndex(db_name, root, root_vector, idx, row);
        }
    }
}

void insertTable(string db_name, vector<string> row){
    vector<vector<string>> info_vector = getFile(db_name, "info");
    int numAtts = stoi(info_vector[1][1]);
    int size = stoi(info_vector[2][1]);
    int index = stoi(info_vector[3][1]);
    string root = info_vector[4][1];

    if(row.size() != numAtts){
        cout<<"invalid update\n";
        return;
    }
    int idx = index - index%BLOCK_SIZE;
    row.emplace(row.begin(),to_string(index));
    insertIndex(db_name, root, idx, row);
    index++;
    size++;

    info_vector[2][1] = to_string(size);
    info_vector[3][1] = to_string(index);
    info_vector[4][1] = root;
    setFile(db_name, "info", info_vector);
}
//--------------------------------------------------------------------------------------------------------------------------------------

vector<string> split(string query){
    vector<string> res;
    string curr = "";
    for(int i=0;i<query.length();i++){
        if(query[i] == ' '){
            res.push_back(curr);
            curr = "";
        }else{
            curr += query[i];
        }
    }
    res.push_back(curr);
    return res;
}

int main(){
    system("clear");
    srand(time(0));
    cout<<"Welcome to File Management System\n";

    cout<<"Some basic commands to operate the system : \n";
    cout<<"exit() -> Exit the system\n";
    
    while(1){
        cout<<"Enter password to access the system : ";
        string password;
        cin>>password;
        cin.ignore();
        if(password == "2615"){
            cout<<"Access Granted!\n";
            break;
        }else if(password == "exit()"){
            return 0;
        }
        cout<<"Access Denied\n";
    }

    system("clear");

    vector<string> dbs = printDBList();

    system("clear");
    
    cout<<"Basic queries :\n";
    cout<<"create db <database_name> (name must contain less than 10 characters) -> Creates a new Database\n";
    cout<<"enter db <database_name> (name must contain less than 10 characters) -> Access the Database\n";
    cout<<"remove db <database_name> (name must contain less than 10 characters) -> Removes the Database\n";
    cout<<"exit db <database_name> (name must contain less than 10 characters) -> Exits the Database\n";
    stack<string> stk;
    while(1){
        cout<<"\n";
        if(!stk.empty()){
            cout<<stk.top();
        }
        cout<<">>";
        string query;
        getline(cin, query);
        if(query == "exit()"){
            break;
        }
        vector<string> q_tokens = split(query);
        if(stk.empty()){
            if(q_tokens.size() != 3){
                cout<<"command not found\n";
            }else if(q_tokens[0] == "create" && q_tokens[1] == "db"){
                string name = q_tokens[2];
                bool found = searchDB(name);
                if(found){
                    cout<<"database of this name already exits\n";
                }else{
                    bool done = createDB(name);
                    if(done){
                        createInfo(name);
                        cout<<"database created\n";
                    }else{
                        cout<<"error\n";
                    }
                }
            }else if(q_tokens[0] == "enter" && q_tokens[1] == "db"){
                string name = q_tokens[2];
                bool found = searchDB(name);
                if(found == false){
                    cout<<"database of this name does not exit\n";
                }else{
                    stk.push(name);
                    cout<<"database opened\n";
                }
            }else if(q_tokens[0] == "remove" && q_tokens[1] == "db"){
                string name = q_tokens[2];
                bool found = searchDB(name);
                if(found == false){
                    cout<<"database of this name does not exits\n";
                }else{
                    string temp = name + "/info.csv";
                    const char* info_path = temp.c_str();
                    if(remove(info_path)){
                        cout<<strerror( errno )<<"\n";
                    }else{
                        cout<<"info removed\n";
                    }

                    removeDB(name);
                }
            }else if(q_tokens[0] == "exit" && q_tokens[1] == "db"){
                cout<<"command invalid\n";
            }else{
                cout<<"command not found\n";
            }
        }else{
            if(q_tokens.size() != 3){
                cout<<"command not found\n";
            }
            // DB queries
            else if(q_tokens[0] == "exit" && q_tokens[1] == "db"){
                string name = q_tokens[2];
                if(name == stk.top()){
                    stk.pop();
                    cout<<"database closed\n";
                }else{
                    cout<<"database of this name is already closed\n";
                }
            }else if((q_tokens[0] == "create" || q_tokens[0] == "remove" || q_tokens[0] == "enter") && q_tokens[1] == "db"){
                cout<<"command invalid\n";
            }
            // Table queries
            else if(q_tokens[0] == "create" && q_tokens[1] == "new" && q_tokens[2] == "table"){
                createSchema(stk.top());
                cout<<"table created\n";
            }else if((q_tokens[0] == "insert") && q_tokens[1] == "into" && q_tokens[2] == "table"){
                cout<<"$ ";
                string update;
                getline(cin,update);
                vector<string> row = split(update);
                insertTable(stk.top(), row);
            }else if((q_tokens[0] == "delete") && q_tokens[1] == "from" && q_tokens[2] == "table"){
                cout<<"$ ";
                string index;
                getline(cin, index);
                bool found = searchIndex(stk.top(), index);
                if(found == false){
                    cout<<"index not found\n";
                }else{
                    cout<<"confirm on deletion (enter y/Y to delete)\n";
                    string confirm;
                    getline(cin, confirm);
                    if(confirm == "y" || confirm == "Y") removeIndex(stk.top(), index);
                }
                
            }else if((q_tokens[0] == "search") && q_tokens[1] == "in" && q_tokens[2] == "table"){
                cout<<"$ ";
                string index;
                getline(cin, index);
                bool found = searchIndex(stk.top(), index);
                if(found == false){
                    cout<<"index not found\n";
                }
            }else if((q_tokens[0] == "remove") && q_tokens[1] == "full" && q_tokens[2] == "table"){
                // Delete all blocks and schema
                // Create new info file
                string name = q_tokens[2];
            }else{
                cout<<"command not found\n";
            }
        }
    }

    return 0;
}