#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;

enum Node_Type{Internal, Leaf};
typedef long ll;
const int t = 10;
const int Min_key_num = t - 1;
const int Max_key_num = 2 * t - 1;

const int Min_child_num = Min_key_num + 1;
const int Max_child_num = Max_key_num + 1;

const int u = 30;
const int Min_data_num = u - 1;//21
const int Max_data_num = 2 * u - 1;//59

class out_of_index
{
    string s;
public:
    out_of_index():s("out_of_index"){cout<<"out of index\n";}
};

class wrong_file
{
    string tmp;
public:
    wrong_file() : tmp("wrong file") {cout<<"wrong file\n";}
};


template<class Key, class Data>
class BPlusTree
{
private:
    int num_of_data;
    FILE *internal_file, *leaf_file, *root_file;
    string file_name1,file_name2,file_name3;
    ll root;
    Node_Type nt_root;

public:
    class L_vac
    {
    public:
        int key_num;
        ll pos;
        ll mypos;
        pair<Key, Data> datas[Max_data_num + 5];
        
        L_vac(ll t = 0) : pos(t){}
    };

    class I_vac
    {
    public:
        int key_num;
        ll pos,mypos;
        Node_Type child_type;
        Key keys[Max_key_num + 5];
        ll childs[Max_child_num + 5];

        I_vac(ll t = 0) : pos(t){}
    };

    void internal_vacancy(ll pos)
    {
        I_vac beg,ne,wh(pos);
        fseek(internal_file, 0,0);
        fread(&beg,sizeof(I_vac),1,internal_file);
        fseek(internal_file, beg.pos, 0);
        fread(&ne,sizeof(I_vac),1,internal_file);
        fseek(internal_file,0,0);
        fwrite(&wh,sizeof(I_vac),1,internal_file);
        fseek(internal_file,pos,0);
        fwrite(&ne,sizeof(I_vac),1,internal_file);
    }
    void leaf_vacancy(ll pos)
    {
        L_vac beg, ne, wh(pos);
        fseek(leaf_file, 0,0);
        fread(&beg,sizeof(L_vac),1,leaf_file);
        fseek(leaf_file, beg.pos, 0);
        fread(&ne,sizeof(L_vac),1,leaf_file);
        fseek(leaf_file,0,0);
        fwrite(&wh,sizeof(L_vac),1,leaf_file);
        fseek(leaf_file,pos,0);
        fwrite(&ne,sizeof(L_vac),1,leaf_file);
    }

    class BPINode;
    class BPLNode;

    void change_par(BPlusTree* bpt,BPINode tmp, int i, BPINode newp)
    {
        if(tmp.child_type == Leaf)
        {
            BPLNode cc(this,tmp.childs[i]);
            cc.par = newp.mypos;
            cc.update(bpt);
        }
        else
        {
            BPINode cc(this,tmp.childs[i]);
            cc.par = newp.mypos;
            cc.update(bpt);
        }
    }

    class BPLNode
    {
    public:
        int key_num;
        ll par;
        ll mypos;
        pair<Key, Data> datas[Max_data_num + 5];

        BPLNode(BPlusTree* bpt, ll k = 0)
        {
            if(k != 0)
            {
                fseek(bpt->leaf_file,k,0);
                fread(this,sizeof(BPLNode),1,bpt->leaf_file);
            }
            else
            {
                par = 0;
                mypos = 0;
                key_num = 0;
            }
        }
        
        void update(BPlusTree* bpt)
        {
            fseek(bpt->leaf_file,mypos,0);
            fwrite(this,sizeof(BPLNode),1,bpt->leaf_file);
        }
        void get(BPlusTree *bpt,ll pos)
        {
            fseek(bpt->leaf_file,pos,0);
            fread(this,sizeof(BPLNode),1,bpt->leaf_file);
        }
        void write(BPlusTree *bpt,ll pos)
        {
            fseek(bpt->leaf_file,pos,0);
            fwrite(this, sizeof(BPLNode),1,bpt->leaf_file);
        }
        void insert(Key key, Data data)
        {
            int i = 1;
            while(i <= key_num && datas[i].first < key)
                i ++;
            for(int j = key_num + 1; j >= i + 1;-- j)
                datas[j] = datas[j - 1];
            datas[i].first = key; datas[i].second = data;
            key_num ++;
        }
        ll file_insert(BPlusTree * bpt)
        {
            L_vac beg, ne;
            fseek(bpt ->leaf_file, 0,0);
            fread(&beg,sizeof(L_vac),1, bpt->leaf_file);
            if(beg.pos == 0)
            {
                fseek(bpt->leaf_file,0,2);
                ll tm = ftell(bpt->leaf_file);
                return tm;
            }
            fseek(bpt->leaf_file, beg.pos, 0);
            fread(&ne,sizeof(L_vac),1, bpt->leaf_file);
            fseek(bpt->leaf_file,0,0);
            fwrite(&ne,sizeof(L_vac),1, bpt->leaf_file);
            return beg.pos;
        }
        ll split_leaf(BPlusTree * bpt,ll pos)
        {
            ll out = par;
            BPLNode tmp(bpt);

            ll pos_of_tmp = file_insert(bpt);
            tmp.par = par;
            tmp.mypos = pos_of_tmp;

            int inter = key_num / 2;
            for(int i = inter + 1;i <= key_num;++ i)
                tmp.datas[i - inter] = datas[i];
            tmp.key_num = key_num - inter;
            key_num = inter;

            if(par == 0)
            {
				BPINode tmppar(bpt);
                tmppar.keys[1] = datas[inter].first;
                tmppar.key_num = 1;
                tmppar.childs[1] = mypos; tmppar.childs[2] = pos_of_tmp;
                tmppar.par = 0;
                tmppar.child_type = Leaf;
                tmppar.mypos = tmppar.file_insert(bpt);
			    par = tmppar.mypos; tmp.par = tmppar.mypos;
                fseek(bpt->internal_file,tmppar.mypos,0);
                fwrite(&tmppar,sizeof(BPINode),1,bpt->internal_file);
                out = par;
            }
            else
            {
				BPINode tmppar(bpt,par);
                tmppar.insert(datas[inter].first, mypos, tmp.mypos);
                fseek(bpt->internal_file,par,0);
                fwrite(&tmppar,sizeof(BPINode),1,bpt->internal_file);
            }
            fseek(bpt -> leaf_file,tmp.mypos,0);
            fwrite(&tmp,sizeof(BPLNode),1,bpt -> leaf_file);
            return out;
        }

        void right_leaf_merge(BPlusTree* bpt,BPLNode tmp,int i)//和它右边的merge
        {
            for(int j = key_num + 1;j <= key_num + tmp.key_num;++ j)
                datas[j] = tmp.datas[j - key_num];
            key_num += tmp.key_num;
			bpt->leaf_vacancy(tmp.mypos);
            fseek(bpt->leaf_file,mypos,0);
            fwrite(this,sizeof(BPLNode),1,bpt->leaf_file);

            BPINode apa(bpt,par);
            apa.key_num --;
            apa.keys[i] = apa.keys[i + 1];
            apa.childs[i] = mypos;

            for(int j = i + 1;j <= apa.key_num;++ j)
            {
                apa.keys[j] = apa.keys[j + 1];
                apa.childs[j] = apa.childs[j + 1];
            }
            if(i != apa.key_num + 1)
                apa.childs[apa.key_num + 1] = apa.childs[apa.key_num + 2];
            fseek(bpt->internal_file,apa.mypos,0);
            fwrite(&apa,sizeof(BPINode),1,bpt->internal_file);
        }   
        void left_leaf_merge(BPlusTree * bpt,BPLNode tmp,int i)
        {
            for(int j = 1;j <= key_num;++ j)
                tmp.datas[tmp.key_num + j] = datas[j];
            tmp.key_num += key_num;
            key_num = tmp.key_num;
            for(int j = 1;j <= key_num; ++ j)
                datas[j] = tmp.datas[j];
			bpt->leaf_vacancy(tmp.mypos);
            fseek(bpt->leaf_file,mypos,0);
            fwrite(this,sizeof(BPLNode),1,bpt->leaf_file);
            
            BPINode apa(bpt,par);
            apa.childs[i - 1] = mypos;
            apa.keys[i - 1] = apa.keys[i];

            for(int j = i;j <= apa.key_num - 1;++ j)
            {
                apa.keys[j] = apa.keys[j + 1];
                apa.childs[j] = apa.childs[j + 1];
            }
            apa.childs[apa.key_num] = apa.childs[apa.key_num + 1];
            apa.key_num --;
            fseek(bpt->internal_file,apa.mypos,0);
            fwrite(&apa,sizeof(BPINode),1,bpt->internal_file);
        }

        void borrow_right_leaf_node(BPlusTree* bpt,BPLNode tmp,int i)
        {
            tmp.key_num --;
            key_num ++;
            datas[key_num] = tmp.datas[1];
            for(int j = 1;j <= tmp.key_num;++ j)
                tmp.datas[j] = tmp.datas[j + 1];
            tmp.update(bpt); update(bpt);
            BPINode apa(bpt,par);
            apa.keys[i] = datas[key_num].first;
            apa.update(bpt);
        }
        void borrow_left_leaf_node(BPlusTree* bpt,BPLNode tmp,int i)
        {
            tmp.key_num --;
            for(int j = key_num;j >= 1;-- j)
                datas[j + 1] = datas[j];
            datas[1] = tmp.datas[tmp.key_num + 1];
            key_num ++;
            tmp.update(bpt); update(bpt);
            BPINode apa(bpt,par);
            apa.keys[i - 1] = (tmp.datas[tmp.key_num]).first;
            apa.update(bpt);
        }

        void adjust(BPlusTree * bpt,int i)
        {
            if(i == 0 || par == 0)
                return;
            BPINode apa(bpt,par);
            if(i == apa.key_num + 1)
            {
                BPLNode tmp = apa.find_leaf_child(bpt, i - 1);
                if(tmp.key_num + key_num <= Max_data_num)
                    left_leaf_merge(bpt,tmp,i);
                else
                    borrow_left_leaf_node(bpt,tmp,i);
            }
            else
            {
                BPLNode tmp = apa.find_leaf_child(bpt,i + 1);
                if(tmp.key_num + key_num <= Max_data_num)
                    right_leaf_merge(bpt,tmp,i);
                else
                    borrow_right_leaf_node(bpt,tmp,i);
            }
        }

    };

    class BPINode
    {
    public:
        int key_num;
        ll par,mypos;
        Node_Type child_type;
        Key keys[Max_key_num + 5];  //use 1 ~ Max_key_num, Max_key_num + 1 is for split
        ll childs[Max_child_num + 5];

        BPINode(BPlusTree* bpt,ll k = 0)
        {
            if(k != 0)
            {
                fseek(bpt->internal_file,k,0);
                fread(this,sizeof(BPINode),1,bpt->internal_file);
            }
        }
               
        void update(BPlusTree * bpt)
        {
            fseek(bpt->internal_file,mypos,0);
            fwrite(this,sizeof(BPINode),1,bpt->internal_file);
        }
        void get(BPlusTree *bpt,ll pos)
        {
            fseek(bpt->internal_file,pos,0);
            fread(this,sizeof(BPLNode),1,bpt->internal_file);
        }
        BPINode find_internal_child(BPlusTree* bpt,int i)
        {
            BPINode tmp(bpt,childs[i]);
            return tmp;
        }   
        BPLNode find_leaf_child(BPlusTree* bpt,int i)
        {
            BPLNode tmp(bpt,childs[i]);
            return tmp;
        }
        ll file_insert(BPlusTree* bpt)
        {
            I_vac beg, ne;
            fseek(bpt->internal_file, 0,0);
            fread(&beg,sizeof(I_vac),1, bpt->internal_file);
            if(beg.pos == 0)
            {
                fseek(bpt->internal_file,0,2);
                ll tm = ftell(bpt->internal_file);
                return tm;
            }
            fseek(bpt->internal_file, beg.pos, 0);
            fread(&ne,sizeof(I_vac),1, bpt->internal_file);
            fseek(bpt->internal_file,0,0);
            fwrite(&ne,sizeof(I_vac),1, bpt->internal_file);
            return beg.pos;
        }
        void write(BPlusTree *bpt,ll pos)
        {
            fseek(bpt->internal_file,pos,0);
            fwrite(this, sizeof(BPINode),1,bpt->internal_file);
        }
        void insert(Key key,ll c1,ll c2)
        {
            int i = 1;
            while(i <= key_num && keys[i] < key)
                i ++;
            if(i == key_num + 1)
            {
                key_num ++;
                keys[i] = key;
                childs[i] = c1;
                childs[i + 1] = c2;
                return;
            }
            for(int j = key_num + 1; j >= i + 1;-- j)
            {
                keys[j] = keys[j - 1];
                childs[j + 1] = childs[j];
            }
            keys[i] = key;
            childs[i + 1] = c2;
            childs[i] = c1;
            key_num ++;
        }

        ll split_internal(BPlusTree *bpt,ll pos)
        {
            BPINode tmp(bpt);
            tmp.child_type = child_type;
            tmp.par = par;
            tmp.mypos = file_insert(bpt);
            int inter = key_num / 2;//去掉keys[inter]
            for(int i = inter + 1;i <= key_num;++ i)
            {
                tmp.keys[i - inter] = keys[i];
                tmp.childs[i - inter] = childs[i];
                bpt->change_par(bpt,*this,i,tmp);
                childs[i] = 0;
            }
		    tmp.childs[key_num - inter + 1] = childs[key_num + 1];
            bpt->change_par(bpt,tmp,key_num - inter + 1,tmp);
        
            childs[key_num + 1] = 0;
            tmp.key_num = key_num - inter;
            key_num = inter - 1;

            if(par == 0)
            {
                BPINode tmppar(bpt);
                tmppar.keys[1] = keys[inter];
                tmppar.key_num = 1;
                tmppar.childs[1] = mypos; tmppar.childs[2] = tmp.mypos;
                tmppar.par = 0; tmppar.child_type = Internal;
                fseek(bpt->internal_file,tmp.mypos,0);
                fwrite(&tmp,sizeof(BPINode),1,bpt->internal_file);
                tmppar.mypos = file_insert(bpt);
    		    par = tmppar.mypos; tmp.par = tmppar.mypos;
                fseek(bpt->internal_file,tmppar.mypos,0);
                fwrite(&tmppar,sizeof(BPINode),1,bpt->internal_file);
                ll out = par;
                fseek(bpt->internal_file,tmp.mypos,0);
                fwrite(&tmp,sizeof(BPINode),1,bpt->internal_file);
                return out;
            }
            else
            {
                BPINode tt(bpt,par);
                tt.insert(keys[inter], mypos, tmp.mypos);
                fseek(bpt->internal_file,tmp.mypos,0);
                fwrite(&tmp,sizeof(BPINode),1,bpt->internal_file);
                fseek(bpt->internal_file,tt.mypos,0);
                fwrite(&tt,sizeof(BPINode),1,bpt->internal_file);
                return par;
            }
			
        }
     
        void right_internal_merge(BPlusTree* bpt,BPINode tmp, int i)//和它右边的merge
        {
            BPINode apa(bpt,par);
            keys[key_num + 1] = apa.keys[i];
            for(int j = key_num + 2;j <= key_num + tmp.key_num + 1;++ j)
            {
                keys[j] = tmp.keys[j - key_num - 1];
                childs[j] = tmp.childs[j - key_num - 1];
                bpt ->change_par(bpt,tmp,j - key_num - 1,*this);
            }

            childs[key_num + tmp.key_num + 2] = tmp.childs[tmp.key_num + 1];
			bpt->change_par(bpt,tmp,tmp.key_num + 1,*this);
            
            key_num += tmp.key_num + 1;
			bpt->internal_vacancy(tmp.mypos);
            update(bpt);
            
            apa.keys[i] = apa.keys[i + 1];
            apa.childs[i] = mypos;
            for(int j = i + 1;j <= apa.key_num - 1; ++ j)
            {
                apa.keys[j] = apa.keys[j + 1];
                apa.childs[j] = apa.childs[j + 1];
            }
            if(apa.key_num != i)
                apa.childs[apa.key_num] = apa.childs[apa.key_num + 1];
            apa.childs[apa.key_num + 1] = 0;
            apa.key_num --;
            apa.update(bpt);
        }
        void left_internal_merge(BPlusTree* bpt,BPINode tmp, int i)
        {
            tmp.childs[tmp.key_num + key_num + 2] = childs[key_num + 1];
            for(int j = 1;j <= key_num;++ j)
            {
                tmp.keys[tmp.key_num + 1 + j] = keys[j];
                tmp.childs[tmp.key_num + 1 + j] = childs[j];
            }
            BPINode apa(bpt,par);
            tmp.keys[tmp.key_num + 1] = apa.keys[i - 1];
            tmp.key_num += key_num + 1;
            key_num = tmp.key_num;
            childs[key_num + 1] = tmp.childs[key_num + 1];

            bpt ->change_par(bpt,tmp,key_num + 1,*this);
            for(int j = 1;j <= key_num;++ j)
            {
                keys[j] = tmp.keys[j];
                childs[j] = tmp.childs[j];
                bpt ->change_par(bpt,tmp,j,*this);
            }
            bpt ->internal_vacancy(tmp.mypos);
            update(bpt);
        
            apa.childs[i - 1] = mypos;
            apa.keys[i - 1] = apa.keys[i];

            for(int j = i;j <= apa.key_num - 1;++ j)
            {
                apa.keys[j] = apa.keys[j + 1];
                apa.childs[j] = apa.childs[j + 1];
            }
            apa.childs[apa.key_num] = apa.childs[apa.key_num + 1];
            apa.key_num --;
            apa.update(bpt);
        }

        void borrow_right_internal_node(BPlusTree* bpt,BPINode tmp, int i)
        {
            BPINode apa(bpt,par);
            keys[key_num + 1] = apa.keys[i];
            key_num ++;
            childs[key_num + 1] = tmp.childs[1];
            bpt ->change_par(bpt,tmp,key_num + 1,*this);
            apa.keys[i] = tmp.keys[1];
            for(int j = 1;j <= tmp.key_num - 1;++ j)
            {
                tmp.keys[j] = tmp.keys[j + 1];
                tmp.childs[j] = tmp.childs[j + 1];
            }
            tmp.childs[tmp.key_num] = tmp.childs[tmp.key_num + 1];
            tmp.childs[tmp.key_num + 1] = 0;
            tmp.key_num --;
            tmp.update(bpt);
            update(bpt);
            apa.update(bpt);
        }
        void borrow_left_internal_node(BPlusTree* bpt,BPINode tmp, int i)
        {
            BPINode apa(bpt,par);
            childs[key_num + 2] = childs[key_num + 1];
        	for(int j = key_num + 1;j >= 2;-- j)
            {
                keys[j] = keys[j - 1];
                childs[j] = childs[j - 1];
            }
            key_num ++;
            childs[1] = tmp.childs[tmp.key_num + 1];
            bpt ->change_par(bpt,tmp,tmp.key_num + 1, *this);
            keys[1] = apa.keys[i - 1];
            apa.keys[i - 1] = tmp.keys[tmp.key_num];
            tmp.childs[tmp.key_num + 1] = 0;
            tmp.key_num --;
            tmp.update(bpt); update(bpt); apa.update(bpt);
        }

        void adjust(BPlusTree *bpt,int i)
        {
            if(i == 0 || par == 0)
                return;
            BPINode apa(bpt,par);
            if(i == apa.key_num + 1)
            {
                BPINode tmp(bpt);
                tmp = apa.find_internal_child(bpt,i - 1);
                if(tmp.key_num + key_num <= Max_key_num)
                    left_internal_merge(bpt,tmp, i);
                else
                    borrow_left_internal_node(bpt,tmp, i);
            }
            else
            {
                BPINode tmp(bpt);
                tmp = apa.find_internal_child(bpt,i + 1);
                if(tmp.key_num + key_num <= Max_key_num)
                    right_internal_merge(bpt,tmp, i);
                else
                    borrow_right_internal_node(bpt,tmp, i);
            }
        }
    };

public:
    BPlusTree(string file1 = "file1", string file2 = "file2", string file3 = "file3")
    {
        root = 0;
        num_of_data = 0;
        nt_root = Leaf;
        file_name1 = file1;
        file_name2 = file2;
        file_name3 = file3;
        const char* F1 = file1.c_str();
        const char* F2 = file2.c_str();
        const char* F3 = file3.c_str();
        fstream _file;
        _file.open(F1, ios::in);
        if(!_file)
        {
            _file.close();
            internal_file = fopen(F1,"wb+");
        }
        else
        {
            _file.close();
            internal_file = fopen(F1, "rb+");
        }
        _file.open(F2, ios::in);
        if(!_file)
        {
            _file.close();
            leaf_file = fopen(F2,"wb+");
        }
        else
        {
            _file.close();
            leaf_file = fopen(F2, "rb+");
        }
        _file.open(F3, ios::in);
        if(!_file)
        {
            _file.close();
            root_file = fopen(F3,"wb+");
            fseek(root_file,0,0);
            fwrite(&root,sizeof(ll),1,root_file);
            fwrite(&num_of_data,sizeof(int),1,root_file);
            fwrite(&nt_root,sizeof(Node_Type),1,root_file);
        }
        else
        {
            _file.close();
            root_file = fopen(F3, "rb+");
            fseek(root_file,0,0);
            fread(&root,sizeof(ll),1,root_file);
            fread(&num_of_data,sizeof(int),1,root_file);
            fread(&nt_root,sizeof(Node_Type),1,root_file);
        }
        L_vac kkk(0);
        I_vac mmm(0);
        fwrite(&kkk,sizeof(L_vac),1,leaf_file);
        fwrite(&mmm,sizeof(I_vac),1,internal_file);
    }
    ~BPlusTree()
    {
        fseek(root_file,0,0);
        fwrite(&root,sizeof(ll),1,root_file);
        fwrite(&num_of_data,sizeof(int),1,root_file);
        fwrite(&nt_root,sizeof(Node_Type),1,root_file);
        //clear();
        fclose(internal_file);
        fclose(leaf_file);
        fclose(root_file);
    }

    void insert(Key key, Data data)
    {
        if(num_of_data == 0)
        {
            BPLNode tmp(this);
            tmp.key_num = 1;
            (tmp.datas[1]).first = key; (tmp.datas[1]).second = data;
            num_of_data ++;
            ll pp = tmp.file_insert(this);
            tmp.mypos = pp;
			tmp.write(this, pp);
            //fseek(leaf_file,pp,0);
            //fwrite(&tmp, sizeof(BPLNode),1,leaf_file);
            root = tmp.mypos;
            return;
        }
        else
        {
            num_of_data ++;
            if(nt_root == Leaf)
            {
                BPLNode tmp(this,root);
                tmp.insert(key, data);
                ll k; bool kkk = 0;
                if(tmp.key_num >= Max_data_num)
                {
                    k = tmp.split_leaf(this, root);
                    kkk = 1;
                }
                tmp.write(this, root);
				if (kkk)
				{
					root = k;
					nt_root = Internal;
				}
            }
            else
            {
                //从root开始向下，如果那个等于Max了就先split一下
                BPINode tmp(this,root);
				ll k;
                if(tmp.key_num >= Max_key_num)
                {
                    k = tmp.split_internal(this, root);
                    tmp.write(this, root);
                    root = k;
                    tmp.get(this,root);
                }
                
                while(tmp.child_type != Leaf)
                {
                    int i = 1;
                    if(tmp.mypos != root && tmp.key_num >= Max_key_num)
                    {
                        k = tmp.split_internal(this, tmp.mypos);
                        tmp.write(this, tmp.mypos);
                        tmp.get(this,k);
                    }
                    while(i <= tmp.key_num && tmp.keys[i] < key)
                        i ++;
                    tmp = tmp.find_internal_child(this, i);
                }
                if(tmp.mypos != root && tmp.key_num >= Max_key_num)
                {
                    k = tmp.split_internal(this, tmp.mypos);
                    tmp.write(this, tmp.mypos);
                    tmp.get(this,k);
                    int i = 1;
                    while(i <= tmp.key_num && tmp.keys[i] < key)
                        i ++;
                    tmp = tmp.find_internal_child(this, i);
                }
                int i = 1;
                BPLNode ttmp(this);
                while(i <= tmp.key_num && tmp.keys[i] < key)
                    i ++;
                ttmp = tmp.find_leaf_child(this, i);
                ttmp.insert(key, data);
                if(ttmp.key_num > Max_data_num)
                    ttmp.split_leaf(this, ttmp.mypos);
                ttmp.write(this, ttmp.mypos);
            }
        }
        return;
    }
    void remove(Key key)
    {
		num_of_data --;
        if(nt_root == Leaf)
        {
            BPLNode tmp(this,root);
            if(tmp.key_num == 0)
                clear();
        }
        else if(nt_root == Internal)
        {
            BPINode tmp(this,root);
            if(tmp.key_num == 0)
            {
                if(tmp.child_type == Leaf)
                {
                    nt_root = Leaf;
                    internal_vacancy(root);
                    root = tmp.childs[1];
                    BPLNode ch = tmp.find_leaf_child(this, 1);
                    ch.par = 0;
                    fseek(leaf_file,ch.mypos,0);
                    fwrite(&ch,sizeof(BPLNode),1,leaf_file);
                }
                else
                {
                    root = tmp.childs[1];
                    internal_vacancy(tmp.mypos);
                    BPINode ch = tmp.find_internal_child(this, 1);
                    ch.par = 0;
                    fseek(internal_file,ch.mypos,0);
                    fwrite(&ch,sizeof(BPINode),1,internal_file);
                }
            }
            else if (tmp.key_num == 1 && tmp.child_type == Leaf)
            {
                BPLNode c1(this),c2(this);
                c1 = tmp.find_leaf_child(this, 1); c2 = tmp.find_leaf_child(this, 2);
                if(c1.key_num + c2.key_num <= Max_data_num)
                {
                    internal_vacancy(root);
                    BPLNode newo(this);
                    newo.mypos = newo.file_insert(this);
                    newo.par = 0;
                    newo.key_num = c1.key_num + c2.key_num;
                    for(int i = 1;i <= c1.key_num;++ i)
                        newo.datas[i] = c1.datas[i];
                    for(int i = 1;i <= c2.key_num;++ i)
                        newo.datas[c1.key_num + i] = c2.datas[i];
                    leaf_vacancy(c1.mypos); leaf_vacancy(c2.mypos);
                    fseek(leaf_file,newo.mypos,0);
                    fwrite(&newo,sizeof(BPLNode),1,leaf_file);
                }
            }
            else if(tmp.key_num == 1 && tmp.child_type == Internal)
            {
                BPINode c1(this),c2(this);
                c1 = tmp.find_internal_child(this, 1); c2 = tmp.find_internal_child(this, 2);
                if(c1.key_num + c2.key_num < Max_key_num)
                {
                    tmp.key_num = c1.key_num + c2.key_num + 1;
                    tmp.keys[c1.key_num + 1] = tmp.keys[1];
                    for(int i = 1;i <= c1.key_num;++ i)
                    {
                        tmp.keys[i] = c1.keys[i];
                        tmp.childs[i] = c1.childs[i];
                        if(c1.child_type == Leaf)
                        {
                            BPLNode mm(this);
                            mm = c1.find_leaf_child(this, i);
                            mm.par = tmp.mypos;
                            fseek(leaf_file,mm.mypos,0);
                            fwrite(&mm,sizeof(BPLNode),1,leaf_file);
                        }
                        else
                        {
                            BPINode mm(this);
                            mm = c1.find_internal_child(this, i);
                            mm.par = tmp.mypos;
                            fseek(internal_file,mm.mypos,0);
                            fwrite(&mm,sizeof(BPINode),1,internal_file);
                        }
                    }
                    tmp.childs[c1.key_num + 1] = c1.childs[c1.key_num + 1];
                    int i = c1.key_num + 1;
                    if(c1.child_type == Leaf)
                    {
                        BPLNode mm(this);
                        mm = c1.find_leaf_child(this, i);
                        mm.par = tmp.mypos;
                        fseek(leaf_file,mm.mypos,0);
                        fwrite(&mm,sizeof(BPLNode),1,leaf_file);
                    }
                    else
                    {
                        BPINode mm(this);
                        mm = c1.find_internal_child(this, i);
                        mm.par = tmp.mypos;
                        fseek(internal_file,mm.mypos,0);
                        fwrite(&mm,sizeof(BPINode),1,internal_file);
                    }
                    for(int i = 1;i <= c2.key_num;++ i)
                    {
                        tmp.keys[i + c1.key_num + 1] = c2.keys[i];
                        tmp.childs[i + c1.key_num + 1] = c2.childs[i];
                        if(c2.child_type == Leaf)
                        {
                            BPLNode mm(this);
                            mm = c2.find_leaf_child(this, i);
                            mm.par = tmp.mypos;
                            fseek(leaf_file,mm.mypos,0);
                            fwrite(&mm,sizeof(BPLNode),1,leaf_file);
                        }
                        else
                        {
                            BPINode mm(this);
                            mm = c2.find_internal_child(this, i);
                            mm.par = tmp.mypos;
                            fseek(internal_file,mm.mypos,0);
                            fwrite(&mm,sizeof(BPINode),1,internal_file);
                        }
                    }
                    tmp.childs[tmp.key_num + 1] = c2.childs[c2.key_num + 1];
                    int ii = c2.key_num + 1;
                    if(c2.child_type == Leaf)
                    {
                        BPLNode mm(this);
                        mm = c2.find_leaf_child(this, ii);
                        mm.par = tmp.mypos;
                        fseek(leaf_file,mm.mypos,0);
                        fwrite(&mm,sizeof(BPLNode),1,leaf_file);
                    }
                    else
                    {
                        BPINode mm(this);
                        mm = c2.find_internal_child(this, ii);
                        mm.par = tmp.mypos;
                        fseek(internal_file,mm.mypos,0);
                        fwrite(&mm,sizeof(BPINode),1,internal_file);
                    }
                    internal_vacancy(c1.mypos);
                    internal_vacancy(c2.mypos);
                    fseek(internal_file,tmp.mypos,0);
                    fwrite(&tmp,sizeof(BPINode),1,internal_file);
                }
            }
        }
        if(nt_root == Leaf)
        {
            BPLNode tmp(this,root);
            int i = 1;
            while(i <= tmp.key_num && (tmp.datas[i]).first != key)
                i ++;
            if(i == tmp.key_num + 1)
                throw out_of_index();
            tmp.key_num --;
            for(int j = i;j <= tmp.key_num;++ j)
                tmp.datas[j] = tmp.datas[j + 1];
            fseek(leaf_file,root,0);
            fwrite(&tmp,sizeof(BPLNode),1,leaf_file);
            return;
        }
		BPINode tmp(this,root);
        int number = 0;
        while(tmp.child_type != Leaf)
        {
            int i = 1;
			if (tmp.key_num <= Min_key_num && tmp.mypos != root)
			{
                tmp.adjust(this,number);
                //tmp = tmp -> par;
            }  
            while(i <= tmp.key_num && tmp.keys[i] < key)
                i ++;
            tmp = tmp.find_internal_child(this, i);
            number = i;
        }
        int i = 1;
        if(tmp.key_num <= Min_key_num && tmp.mypos != root)
            tmp.adjust(this, number);
        while(i <= tmp.key_num && tmp.keys[i] < key)
            i ++;
        BPLNode tmpp(this);
        tmpp = tmp.find_leaf_child(this, i);
        i = 1;

        while(i <= tmpp.key_num && (tmpp.datas[i]).first != key)
            i ++;
        if(i == tmpp.key_num + 1)
            throw out_of_index();
        tmpp.key_num --;
        for(int j = i;j <= tmpp.key_num;++ j)
            tmpp.datas[j] = tmpp.datas[j + 1];
        if(tmpp.key_num < Min_data_num && tmpp.mypos != root)
            tmpp.adjust(this, number);
        fseek(leaf_file,tmpp.mypos,0);
        fwrite(&tmpp,sizeof(BPLNode),1,leaf_file);
		return;
    }
    bool find(Key key)
    {
        if(root == 0) return 0;
        BPLNode tmp(this);
        if(nt_root == Leaf)
            tmp.get(this,root);
        else
        {
            BPINode ttmp(this,root);
            while(ttmp.child_type == Internal)
            {
                int i = 1;
                while(i <= ttmp.key_num && ttmp.keys[i] < key)
                    i ++;
                ttmp = ttmp.find_internal_child(this, i);
            }
            int i = 1;
            while(i <= ttmp.key_num && ttmp.keys[i] < key)
                i ++;
            tmp = ttmp.find_leaf_child(this, i);
        }
        int i = 1;
        while((tmp.datas[i]).first != key && i <= tmp.key_num)
            i ++;
        if(i == tmp.key_num + 1)
            return false;
        else 
            return true;
    }
    Data operator [](Key key)
    {
        if(root == 0) throw out_of_index();
        BPLNode tmp(this);
        if(nt_root == Leaf)
            tmp.get(this,root);
        else
        {
            BPINode ttmp(this,root);
            while(ttmp.child_type == Internal)
            {
                int i = 1;
                while(i <= ttmp.key_num && ttmp.keys[i] < key)
                    i ++;
                ttmp = ttmp.find_internal_child(this,i);
            }
            int i = 1;
            while(i <= ttmp.key_num && ttmp.keys[i] < key)
                i ++;
            tmp = ttmp.find_leaf_child(this,i);
        }
        int i = 1;
        while((tmp.datas[i]).first != key && i <= tmp.key_num)
            i ++;
        if(i == tmp.key_num + 1)
            throw out_of_index();
        else 
            return (tmp.datas[i]).second;
    }
    void update_data(Key key, Data new_data)
    {
        ll ppos = root;
        if(root == 0) throw out_of_index();
        BPLNode tmp(this);
        if(nt_root == Leaf)
            tmp.get(this,root);
        else
        {
            BPINode ttmp(this,root);
            while(ttmp.child_type == Internal)
            {
                int i = 1;
                while(i <= ttmp.key_num && ttmp.keys[i] < key)
                    i ++;
                ttmp = ttmp.find_internal_child(this, i);
                ppos = ttmp.childs[i];
            }
            int i = 1;
            while(i <= ttmp.key_num && ttmp.keys[i] < key)
                i ++;
            tmp = ttmp.find_leaf_child(this, i);
            ppos = ttmp.childs[i];
        }
        int i = 1;
        while((tmp.datas[i]).first != key && i <= tmp.key_num)
            i ++;
        if(i == tmp.key_num + 1)
            throw out_of_index();
        else 
            (tmp.datas[i]).second = new_data;
        tmp.write(this,ppos);
    }
    void clear()
    {
        if(num_of_data == 0) return;
        fclose(internal_file);
        fclose(leaf_file);
        fclose(root_file);
        const char* F1 = file_name1.c_str();
        const char* F2 = file_name2.c_str();
        const char* F3 = file_name3.c_str();
        internal_file = fopen(F1, "wb+");
        leaf_file = fopen(F2, "wb+");
        root_file = fopen(F3,"wb+");
        root = 0;
        L_vac tmp(0);
        I_vac ttmp(0);
        fwrite(&tmp,sizeof(L_vac),1,leaf_file);
        fwrite(&ttmp,sizeof(I_vac),1,internal_file);
        fwrite(&root,sizeof(ll),1,root_file);
        fwrite(&num_of_data,sizeof(int),1,root_file);
        fwrite(&nt_root,sizeof(Node_Type),1,root_file);
        num_of_data = 0;
    }
    
};

#endif;

/*
一些优化or问题:
·查找还可以优化成二分查找
·大数据点还没测试(包括大数据量,大u,大t)
·把u放进模板里
*/

/*
增添一个root_type用来确认root在哪个文件里
root指代root节点在其对应的文件的位置

对于bpnode：
internal文件中的节点都是internal节点
存储顺序为
int key_num; BPlus_Node<Key, Data> * par（指代其par在internal文件中的位置）;
node_type child_type（指其孩子的类型以定位其孩子所在的文件）;
Key keys[Max_key_num + 5];  //use 1 ~ Max_key_num, Max_key_num + 1 is for split
BPlus_Node<Key, Data> * childs[Max_child_num + 5]（指代其孩子在对应文件中的位置）;

leaf文件中的节点都是leaf节点
存储顺序为
int key_num;
BPlus_Node<Key, Data> * par(记录父亲节点在internal文件中的位置);
BPlus_Node<Key, Data> * Left_Sibling, * Right_Sibling(记录这两个节点在叶子中的位置);
pair<Key, Data> datas[Max_data_num + 5](可以不new直接来);

*/