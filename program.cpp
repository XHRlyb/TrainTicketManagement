#include <bits/stdc++.h>
using namespace std;

#include "BPlus_Tree_8.hpp"
//#include "BPlus_Tree_debug3.hpp"

typedef long long LL;
const int K1 = 21;
const int K2 = 31;
const int N = 110;
const int N_ = 1e4 + 10;
const int M = 1e6 + 10;

int Tot(0);

template<class Type>
Type get_kth_data(int k,string file_name)//在file_name文件中取第k个元素
{
    const char* F1 = file_name.c_str();
    FILE * f;
    Type out;
    fstream _file;
    _file.open(F1, ios::in);
    if(!_file)
        f = fopen(F1,"wb+");
    else
        f = fopen(F1, "rb+");
    fseek(f,(k - 1) * sizeof(Type),0);
    fread(&out,sizeof(Type),1,f);
    fclose(f);
    return out;
}

template<class Type>
void add_to_file3(Type data,int k,string file_name) //在file_name文件第k个位置加入data元素
{
    const char* F1 = file_name.c_str();
    FILE * f;
    fstream _file;
    _file.open(F1, ios::in);
    if(!_file)
        f = fopen(F1,"wb+");
    else
        f = fopen(F1, "rb+");
    fseek(f,(k - 1) * sizeof(Type),0);
    fwrite(&data,sizeof(Type),1,f);
    fclose(f);
}

struct tim{
	int h, m;
	bool operator < (const tim &b) const {
		return (h < b.h || (h == b.h && m < b.m));
	}

	bool operator <= (const tim &b) {
		return (h < b.h || (h == b.h && m <= b.m));
	}

	int operator - (const tim &b){
		return (m + h * 60 - b.m - b.h * 60);
	}

	tim operator + (const int &d){
		tim t;
		t.m = m + d;
		t.h = (h + t.m / 60) % 24;
		t.m %= 60;
		return t;
	}

	int calc(int d){
		return (h + (m + d) / 60) / 24;
	}

	void output(){
		printf("%.2d:%.2d", h, m);
	}
};

struct date{
	int m, d;

	bool operator < (const date &b) const {
		return (m < b.m || (m == b.m && d < b.d));
	}

	bool operator <= (const date &b){
		return (m < b.m || (m == b.m && d <= b.d));
	}

	bool operator == (const date &b){
		return (m == b.m && d == b.d);
	}

	int operator - (const date &b){
		int x = d, y = b.d;
		if (m > 6)
			x += 30;
		if (m > 7)
			x += 31;
		if (m > 8)
			x += 31;
		if (b.m > 6)
			y += 30;
		if (b.m > 7)
			y += 31;
		if (b.m > 8)
			y += 31;
		return x - y;
	}

	date operator + (const int &x){
		date t;
		t.m = m;
		t.d = d + x;
		if (t.m == 6 && t.d > 30)
			t.d -= 30, t.m++;
		if (t.m == 7 && t.d > 31)
			t.d -= 31, t.m++;
		if (t.m == 8 && t.d > 31)
			t.d -= 31, t.m++;
		return t;
	}

	void copy(const date &b){
		m = b.m;
		d = b.d;
	}

	void output(){
		printf("%.2d-%.2d", m, d);
	}
};

struct Tim{
	date a;
	tim b;

	int operator - (const Tim &d){
		return ((a - d.a) * 24 * 60 + (b - d.b));
	}

	Tim operator + (const int &d){
		Tim t;
		t.a = a;
		t.b = b;
		t.b.m += d;
		if (t.b.m >= 60){
			t.b.h += t.b.m / 60;
			t.b.m = t.b.m % 60;
			if (t.b.h > 23){
				t.a = t.a + t.b.h / 24;
				t.b.h %= 24;
			}
		}
		return t;
	}

	bool operator <= (const Tim &c) {
		return (a < c.a || (a == c.a && b <= c.b));
	}

	void output(){
		printf("%.2d-%.2d %.2d:%.2d", a.m, a.d, b.h, b.m);
	}
};

struct node{
	char id[K1];
	int d, p, nxt, pre;

	node(){
		for (int i = 0; i < K1; i++)
			id[i] = '\0';
		nxt = pre = 0;
	}
};//a[M];

class data{
public:
	int fir, cur, fir_, cur_;
	data(){
		fir = fir_ = 0;
	}

	data(int a, int b, int c, int d) : fir(a), cur(b), fir_(c), cur_(d) {}
};

class Key{
public:
	char id[K1];

	Key(){
		for (int i = 0; i < K1; i++)
			id[i] = '\0';
	}

	bool operator < (const Key &b) const {
		return (strcmp(id, b.id) < 0);
	}

	bool operator == (const Key &b){
		return !strcmp(id, b.id);
	}

	bool operator != (const Key &b){
		return strcmp(id, b.id);
	}
};

class Key_{
public:
	char id[K2];

	Key_(){
		for (int i = 0; i < K2; i++)
			id[i] = '\0';
	}

	bool operator < (const Key_ &b) const {
		return (strcmp(id, b.id) < 0);
	}

	bool operator == (const Key_ &b){
		return !strcmp(id, b.id);
	}

	bool operator != (const Key_ &b){
		return strcmp(id, b.id);
	}
};

class User{
public:
	char pas[K2], name[K1], mail[K2];
	int pri, fir, cur, cnt;
	bool isLog;

	User(){
		isLog = 0;
		fir = cnt = 0;
	}

	void cp(User b){
		strcpy(pas, b.pas);
		strcpy(name, b.name);
		strcpy(mail, b.mail);
		pri = b.pri;
		fir = b.fir;
		cur = b.cur;
		cnt = b.cnt;
		isLog = b.isLog;
	}

	void out(){
		isLog = 0;
	}
};

class Train{
public:
	int num, seat, pri[N], r[N], t[N], s[N], p[N], dm[2][N];
	int fir[N], cur[N];
	char sta[N][K2], ty[3];
	tim st, ar[N], lv[N];
	date S, T;
	bool isr;

	Train(){
		isr = 0;
		memset(fir, 0, sizeof(fir));
	}
};

BPlusTree<Key, User> user("a.txt", "b.txt");
BPlusTree<Key, Train> train("c.txt", "d.txt");
BPlusTree<Key_, data> sta("e.txt", "f.txt");
bool isf(0);
int tota(0), totr(0);
char q[4], ch;
User tu;
Train tt;
data td;

int add_user(){
	Key cur, k;
	User t;

	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'c':
					scanf("%s", cur.id);
					break;
				case 'u':
					scanf("%s", k.id);
					break;
				case 'p':
					scanf("%s", t.pas);
					break;
				case 'n':
					scanf("%s", t.name);
					break;
				case 'm':
					scanf("%s", t.mail);
					break;
				case 'g':
					scanf("%d", &t.pri);
			}
		}
	}

	if (!isf){
		isf = 1;
		t.pri = 10;
		user.insert(k, t);
		return 0;
	}
	if (user.find(cur) && !user.find(k) && user[cur].isLog && t.pri < user[cur].pri){
		user.insert(k, t);
		return 0;
	}
	return -1;
}

struct Log{
	Key k;

    Log(){}
}tl;

int totl(0);

int Login(){
	Key k;
	char pas[K2];
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'u':
					scanf("%s", k.id);
					break;
				case 'p':
					scanf("%s", pas);
					break;
			}
		}
	}

	if (user.find(k) && !user[k].isLog && !strcmp(user[k].pas, pas)){
	//	user[k].isLog = 1;
		tu = user[k];
		tu.isLog = 1;
		user.update_data(k, tu);
        
        tl.k = k;
		add_to_file3(tl, ++totl, "lo.txt");
	//  lo[++totl].k = k;
		return 0;
	}
	return -1;
}

int Logout(){
	Key k;
	scanf("%s%s", q, k.id);
	if (user.find(k) && user[k].isLog){
	//	user[k].isLog = 0;
		tu = user[k];
		tu.isLog = 0;
		user.update_data(k, tu);
		return 0;
	}
	return -1;
}

void query_profile(){
	Key c, k;
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'c':
					scanf("%s", c.id);
					break;
				case 'u':
					scanf("%s", k.id);
					break;
			}
		}
	}

	if (user.find(c) && user.find(k) && user[c].isLog && 
		(!strcmp(c.id, k.id) || user[c].pri > user[k].pri)){
		printf("%s %s %s %d\n", k.id, user[k].name, user[k].mail, user[k].pri);
	}
	else{
		puts("-1");
	}
}

void modify_profile(){
	Key c, k; 
	User t;
	scanf("%s%s", q, c.id);
	scanf("%s%s", q, k.id);
	if (user.find(k))
		t.cp(user[k]);
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			if (ch == 'p')
				scanf("%s", t.pas);
			else
				if (ch == 'n')
					scanf("%s", t.name);
				else
					if (ch == 'm')
						scanf("%s", t.mail);
					else
						scanf("%d", &t.pri);
		}
	}
	if (user.find(c) && user.find(k) && user[c].isLog &&
		(!strcmp(c.id, k.id) || user[c].pri > user[k].pri) && user[c].pri > t.pri){
		user.update_data(k, t);
		printf("%s %s %s %d\n", k.id, user[k].name, user[k].mail, user[k].pri);
	}
	else{
		puts("-1");
	}
}

//-------------------

int ta[N], tb[N];

int add_train(){
	Key k;
	Train t;
	int cnt, len;
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'i':
					scanf("%s", k.id);
					break;
				case 'n':
					scanf("%d", &t.num);
					break;
				case 'm':
					scanf("%d", &t.seat);
					break;
				case 's':
					cnt = 1, len = 0;
					for (ch = getchar(), ch = getchar(); ch != ' '; ch = getchar()){
						if (ch == '|'){
							t.sta[cnt][len] = '\0';
							cnt++, len = 0;
						}
						else
							t.sta[cnt][len++] = ch;
					}
					t.sta[cnt][len] = '\0';
					break;
				case 'p':
					cnt = 2, len = 0;
					t.pri[1] = 0;
					for (ch = getchar(), ch = getchar(); ch != ' '; ch = getchar()){
						if (ch == '|'){
							t.pri[cnt] = len;
							cnt++, len = 0;
						}
						else
							len = len * 10 + ch - '0';
					}
					t.pri[cnt] = len;
					for (int i = 2; i <= t.num; i++)
						t.pri[i] += t.pri[i - 1];
					break;
				case 'x':
					scanf("%d", &t.st.h);
					ch = getchar();
					scanf("%d", &t.st.m);
					break;
				case 't':
					cnt = 2, len = 0;
					ta[1] = 0;
					for (ch = getchar(), ch = getchar(); ch != ' '; ch = getchar()){
						if (ch == '|'){
							ta[cnt] = len;
							cnt++, len = 0;
						}
						else
							len = len * 10 + ch - '0';
					}
					ta[cnt] = len;
					break;
				case 'o':
					cnt = 2, len = 0;
					tb[1] = 0;
					for (ch = getchar(), ch = getchar(); ch != ' '; ch = getchar()){
						if (ch == '|'){
							tb[cnt] = len;
							cnt++, len = 0;
						}
						else
							len = len * 10 + ch - '0';
					}
					tb[cnt] = len;
					break;
				case 'd':
					scanf("%d", &t.S.m);
					ch = getchar();
					scanf("%d", &t.S.d);
					ch = getchar();
					scanf("%d", &t.T.m);
					ch = getchar();
					scanf("%d", &t.T.d);
					break;
				case 'y':
					scanf("%s", t.ty);
					break;
			}
		}
	}

	if (train.find(k))
		return -1;
	
	cnt = t.T - t.S;
	for (int i = 0; i <= cnt; i++){
		t.r[i] = totr;
		for (int j = 1; j <= t.num; j++)
			add_to_file3(t.seat, ++totr, "res.txt");
		//	t.res[i][j] = t.seat;
	}

	tim cur = t.st;
	t.ar[1] = t.lv[1] = t.st;
	t.dm[0][1] = t.dm[1][1] = 0;
	int xt(0);
	for (int i = 2; i <= t.num; i++){
		xt += cur.calc(ta[i]);
		t.dm[0][i] = xt;
		cur = cur + ta[i];
		t.ar[i] = cur;
		if (i == t.num)
			break;
		xt += cur.calc(tb[i]);
		t.dm[1][i] = xt;
		cur = cur + tb[i];
		t.lv[i] = cur;
	}

	Key_ t_;

	for (int i = 1; i <= t.num; i++){
		strcpy(t_.id, t.sta[i]);
		if (sta.find(t_)){
			if (!sta[t_].fir){
				td = sta[t_];
				td.fir = td.cur = ++tota;
				sta.update_data(t_, td);
			//	sta[t_].fir = sta[t_].cur = ++tota;
				node ta;
				strcpy(ta.id, k.id);
				ta.p = i;
				ta.nxt = ta.pre = 0;
				add_to_file3(ta, tota, "node.txt");
			}
			else{
				node ta;
				ta = get_kth_data<node>(sta[t_].cur, "node.txt");
				ta.nxt = ++tota;
				add_to_file3(ta, sta[t_].cur, "node.txt");
				node tb;
				strcpy(tb.id, k.id);
				tb.p = i;
				tb.pre = sta[t_].cur;
				tb.nxt = 0;
				add_to_file3(tb, tota, "node.txt");

				td = sta[t_];
				td.cur = tota;
				sta.update_data(t_, td);
			//	sta[t_].cur = tota;
			}
		}
		else{
			++tota;
			sta.insert(t_, data(tota, tota, 0, 0));
			node ta;
			strcpy(ta.id, k.id);
			ta.p = i;
			ta.nxt = ta.pre = 0;
			add_to_file3(ta, tota, "node.txt");
		}
		t.p[i] = tota;
	}
	//
	train.insert(k, t);
	return 0;
}

int release_train(){
	Key k;
	scanf("%s%s", q, k.id);
	if (train.find(k) && !train[k].isr){
		tt = train[k];
		tt.isr = 1;
		train.update_data(k, tt);
	//	train[k].isr = 1;
		return 0;
	}
	return -1;
}

void query_train(){
	Key k;
	date d;
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'd':
					scanf("%d", &d.m);
					ch = getchar();
					scanf("%d", &d.d);
					break;
				case 'i':
					scanf("%s", k.id);
					break;
			}
		}
	}

	if (!train.find(k)){
		puts("-1");
		return;
	}
	Train t = train[k];
	if (t.S <= d && d <= t.T){
		printf("%s %s\n", k.id, t.ty);
		int de = d - t.S;
		for (int i = 1; i <= t.num; i++){
			printf("%s ", t.sta[i]);
			if (i == 1)
				printf("xx-xx xx:xx");
			else{
				(d + t.dm[0][i]).output();
				printf(" ");
				t.ar[i].output();
			}
			printf(" -> ");
			if (i == t.num)
				printf("xx-xx xx:xx");
			else{
				(d + t.dm[1][i]).output();
				printf(" ");
				t.lv[i].output();
			}
			printf(" %d ", t.pri[i]);
			if (i == t.num)
				puts("x");
			else
				printf("%d\n", get_kth_data<int>(t.r[de] + i, "res.txt"));
		}
	}
	else
		puts("-1");
}

int delete_train(){
	Key k;
	scanf("%s%s", q, k.id);
	Key_ t_;
	if (train.find(k) && !train[k].isr){
		Train t = train[k];
		int x;
		for (int i = 1; i <= t.num; i++){
			x = t.p[i];
			strcpy(t_.id, t.sta[i]);
			node xx = get_kth_data<node>(x, "node.txt");
			if (!xx.pre){
				td = sta[t_];
				td.fir = xx.nxt;
				sta.update_data(t_, td);
			//	sta[t_].fir = a[x].nxt;
				node yy = get_kth_data<node>(xx.nxt, "node.txt");
				yy.pre = 0;
				add_to_file3(yy, xx.nxt, "node.txt");  /*???*/
			}
			else{
				if (!xx.nxt){
					td = sta[t_];
					td.cur = xx.pre;
					sta.update_data(t_, td);
				//	sta[t_].cur = a[x].pre;
					node yy = get_kth_data<node>(xx.pre, "node.txt");
					yy.nxt = xx.nxt;
					add_to_file3(yy, xx.pre, "node.txt");
				}
				else{
					node yy = get_kth_data<node>(xx.pre, "node.txt");
					node zz = get_kth_data<node>(xx.nxt, "node.txt");
					yy.nxt = xx.nxt;
					zz.pre = xx.pre;
					add_to_file3(yy, xx.pre, "node.txt");
					add_to_file3(zz, xx.nxt, "node.txt");
				}
			}
		}
		train.remove(k);	
		return 0;
	}
	return -1;
}

struct Ans{
	char id[K1];
	date S, T;
	tim s, t;
	int x, p, mn;

	bool operator <(const Ans &b) const {
		return x < b.x || (x == b.x && strcmp(id, b.id) < 0);
	}
}ans[N_];

void query_ticket(){
	char S[K2], T[K2];
	Key_ ks, kt;
	date d;
	bool fl(0);
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'd':
					scanf("%d", &d.m);
					ch = getchar();
					scanf("%d", &d.d);
					break;
				case 's':
					scanf("%s", S);
					break;
				case 't':
					scanf("%s", T);
					break;
				case 'p':
					scanf("%s", q);
					fl = (q[0] == 'c');
					break;
			}
		}
	}
	strcpy(ks.id, S);
	strcpy(kt.id, T);

	if (!sta.find(ks) || !sta.find(kt)){
		puts("0");
		return;
	}

	Train t;
	int cnt(0), mn, de;
	Key ts;
	for (int i = sta[ks].fir; i; /*???*/){
		node ii = get_kth_data<node>(i, "node.txt");
		strcpy(ts.id, ii.id);
		t = train[ts];

		if (1){
			if (!t.isr)
				continue;
			mn = 1e9;
			de = d - t.S - t.dm[1][ii.p];
			if (de < 0 || de > t.T - t.S)
				continue;
			for (int j = ii.p; j <= t.num; j++){
				if (!strcmp(t.sta[j], T)){
					strcpy(ans[++cnt].id, ii.id);
					ans[cnt].s = t.lv[ii.p];
					ans[cnt].t = t.ar[j];
					ans[cnt].p = t.pri[j] - t.pri[ii.p];
					ans[cnt].mn = mn;
					ans[cnt].S = d;
					ans[cnt].T = d + (t.dm[0][j] - t.dm[1][ii.p]);
					ans[cnt].x = ((!fl) ? (ans[cnt].T - ans[cnt].S) * 24 * 60 + (ans[cnt].t - ans[cnt].s) : t.pri[j] - t.pri[ii.p]);
					break;
				}
				mn = min(mn, get_kth_data<int>(t.r[de] + j, "res.txt"));
			}
		}
		i = ii.nxt;
	}
	sort(ans + 1, ans + 1 + cnt);
	printf("%d\n", cnt);
	for (int i = 1; i <= cnt; i++){
		printf("%s %s ", ans[i].id, S);
		ans[i].S.output();
		printf(" ");
		ans[i].s.output();
		printf(" -> %s ", T);
		ans[i].T.output();
		printf(" ");
		ans[i].t.output();
		printf(" %d %d\n", ans[i].p, ans[i].mn);
	}
}

struct trans{
	char id[K1];
	int x, y, nxt, p, l, r;
	tim s, t;
	date S, T;

	trans(){
		nxt = 0;
	}
}dt;

struct Trans{
	char id[K1], S[K2], T[K2];
	Tim s, t;
	int p, x;
}ans1, ans2;

int totd(0);

void query_transfer(){
char S[K2], T[K2];
	Key_ ks, kt;
	date D;
	bool fl;
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'd':
					scanf("%d", &D.m);
					ch = getchar();
					scanf("%d", &D.d);
					break;
				case 's':
					scanf("%s", S);
					break;
				case 't':
					scanf("%s", T);
					break;
				case 'p':
					scanf("%s", q);
					fl = (q[0] == 'c');
					break;
			}
		}
	}
	strcpy(ks.id, S);
	strcpy(kt.id, T);

	if (!sta.find(ks) || !sta.find(kt)){
		puts("0");
		return;
	}

	Train t;
	int l, mn;
	totd = 0;
	Key ts, ts_;
	Key_ t_;
	node ta;
	trans Dt;
	for (int i = sta[kt].fir; i; i = get_kth_data<node>(i, "node.txt").nxt){
		ta = get_kth_data<node>(i, "node.txt");
		strcpy(ts.id, ta.id);
		t = train[ts];
		if (t.isr){
			l = 0;
			for (int j = 1; j <= t.num; j++){
				if (!strcmp(t.sta[j], T)){
					l = j;
					break;
				}
			}
			for (int j = l - 1; j; j--){
				
				strcpy(Dt.id, ta.id);
				Dt.nxt = 0;
				Dt.s = t.lv[j];
				Dt.t = t.ar[l];
				Dt.S.copy(t.S);
				Dt.T.copy(t.T);
				Dt.x = t.dm[1][j];
				Dt.y = t.dm[0][l];
				Dt.p = t.pri[l] - t.pri[j];
				Dt.l = j;
				Dt.r = l;
				
				add_to_file3(Dt, ++totd, "trans.txt");

				strcpy(t_.id, t.sta[j]);
				if (!sta[t_].fir_){
					td = sta[t_];
					td.fir_ = td.cur_ = totd;
					sta.update_data(t_, td);
				//	sta[t_].fir_ = sta[t_].cur_ = totd;
				}
				else{
					trans zj = get_kth_data<trans>(sta[t_].cur_, "trans.txt");
					zj.nxt = totd;
					add_to_file3(zj,sta[t_].cur_,"trans.txt");					
					td = sta[t_];
					td.cur_ = totd;
					sta.update_data(t_, td);
				//	sta[t_].cur_ = totd;
				}
			}
		}
	}

	Tim s, L, x, e;
	int de, dt, ans(1e9), mn_, ans_;
	for (int i = sta[ks].fir; i; i = get_kth_data<node>(i, "node.txt").nxt){
		strcpy(ts.id, get_kth_data<node>(i, "node.txt").id);
		t = train[ts];
		if (t.isr){
			l = 0;
			for (int j = 1; j <= t.num; j++){
				if (!strcmp(t.sta[j], S)){
					l = j;
					break;
				}
			}
			if (D < t.S)
				continue;
			de = D - t.S - t.dm[1][l];
			if (de < 0 || de > t.T - t.S)
				continue;
			s.a = D;
			s.b = t.lv[l];
			mn = 1e9;

			for (int j = l + 1; j <= t.num; j++){
				mn = min(mn, get_kth_data<int>(t.r[de] + j - 1, "res.txt"));
				L.a = t.S + (de + t.dm[0][j]);
				L.b = t.ar[j];
				strcpy(t_.id, t.sta[j]);
				for (int k = sta[t_].fir_; k; k = get_kth_data<node>(k, "trans.txt").nxt){
					node ayhi = get_kth_data<node>(i, "node.txt");
					trans dyhk = get_kth_data<trans>(k, "trans.txt");
					if (!strcmp(dyhk.id, ayhi.id))
						continue;
					strcpy(ts_.id, dyhk.id);
					x.a = L.a;
					x.b = dyhk.s;
					if (x.b < L.b)
						x.a = x.a + 1;
					dt = x.a - dyhk.S - dyhk.x;
					if (dt < 0)
						x.a = dyhk.S + dyhk.x, dt = 0;
					if (dt > dyhk.T - dyhk.S)
						continue;
					e.a = x.a + (dyhk.y - dyhk.x);
					e.b = dyhk.t;
					if (!fl && (e - s < ans || (e - s == ans && L - s < ans_))){
						ans = e - s;
						ans_ = L - s;
						strcpy(ans1.id, ayhi.id);
						strcpy(ans2.id, dyhk.id);
						strcpy(ans1.S, S);
						strcpy(ans1.T, t.sta[j]);
						strcpy(ans2.S, t.sta[j]);
						strcpy(ans2.T, T);
						ans1.p = t.pri[j] - t.pri[l];
						ans1.x = mn;
						ans2.p = dyhk.p;
						ans1.s = s;
						ans1.t = L;
						ans2.s = x;
						ans2.t = e;
						mn_ = 1e9;
						int pr = train[ts_].r[dt];  /*?????*/
						for (int o = dyhk.l; o < dyhk.r; o++)
							mn_ = min(mn_, get_kth_data<int>(pr + o, "res.txt"));
						ans2.x = mn_;
					}
					if (fl && (t.pri[j] - t.pri[l] + dyhk.p < ans || (t.pri[j] - t.pri[l] + dyhk.p == ans && L - s < ans_))){
						ans = t.pri[j] - t.pri[l] + dyhk.p;
						ans_ = L - s;
						strcpy(ans1.id, ayhi.id);
						strcpy(ans2.id, dyhk.id);
						strcpy(ans1.S, S);
						strcpy(ans1.T, t.sta[j]);
						strcpy(ans2.S, t.sta[j]);
						strcpy(ans2.T, T);
						ans1.p = t.pri[j] - t.pri[l];
						ans1.x = mn;
						ans2.p = dyhk.p;
						ans1.s = s;
						ans1.t = L;
						ans2.s = x;
						ans2.t = e;
						mn_ = 1e9;
						int pr = train[ts_].r[dt]; /*?????*/
						for (int o = dyhk.l; o < dyhk.r; o++)
							mn_ = min(mn_, get_kth_data<int>(pr + o, "res.txt"));
						ans2.x = mn_;
					}
					add_to_file3(dyhk,k,"trans.txt");
					add_to_file3(ayhi,i,"node.txt");
				}
			}
		}
	}

	if (ans != 1e9){
		printf("%s %s ", ans1.id, ans1.S);
		ans1.s.output();
		printf(" -> %s ", ans1.T);
		ans1.t.output();
		printf(" %d %d\n", ans1.p, ans1.x);
		printf("%s %s ", ans2.id, ans2.S);
		ans2.s.output();
		printf(" -> %s ", T);
		ans2.t.output();
		printf(" %d %d\n", ans2.p, ans2.x);
	}
	else
		puts("0");

	for (int i = sta[kt].fir; i; i = get_kth_data<node>(i,"node.txt").nxt){
		strcpy(ts.id, get_kth_data<node>(i,"node.txt").id);
		t = train[ts];
		if (t.isr){
			for (int j = 1; j <= t.num; j++){
				if (!strcmp(t.sta[j], T))
					break;
				strcpy(t_.id, t.sta[j]);

				td = sta[t_];
				td.fir_ = 0;
				sta.update_data(t_, td);
			//	sta[t_].fir_ = 0;
			}
		}
	}
	
}

struct Order{
	Key c;
	char id[K1], S[K2], T[K2];
	date ds, dt;
	tim s, t;
	int x, p, ty, nxt, pre_, nxt_, ps, pt, d;
	Order(){
		nxt = pre_ = nxt_ = 0;
	}
};//order[M];

int totb(0);

void buy_ticket(){
	char us[K1], tr[K1], S[K2], T[K2];
	Key c, k;
	date d;
	int x;
	bool fl_(0);
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'd':
					scanf("%d", &d.m);
					ch = getchar();
					scanf("%d", &d.d);
					break;
				case 'u':
					scanf("%s", us);
					break;
				case 'i':
					scanf("%s", tr);
					break;
				case 'f':
					scanf("%s", S);
					break;
				case 't':
					scanf("%s", T);
					break;
				case 'n':
					scanf("%d", &x);
					break;
				case 'q':
					scanf("%s", q);
					fl_ = (q[0] == 't');
					break;
			}
		}
	}
	strcpy(c.id, us);
	strcpy(k.id, tr);

	if (!user.find(c) || !user[c].isLog || !train.find(k) || !train[k].isr){
		puts("-1");
		return;
	}

	Train t = train[k];
	int fl(0), de, mn,rt,pr;
	for (int i = 1; i <= t.num; i++){
		if (!strcmp(t.sta[i], T))
			fl = 1;
		if (!strcmp(t.sta[i], S)){
			if (fl){
				puts("-1");
				return;
			}
			de = d - t.S - t.dm[1][i];
			if (de < 0 || de > t.T -  t.S)
				break;
			mn = 1e9;
			for (int j = i; j <= t.num; j++){
				if (!strcmp(t.sta[j], T)){
					fl = j;
					break;
				}
				mn = min(mn, get_kth_data<int>(t.r[de] + j, "res.txt"));
				if (mn < x && !fl_){
					puts("-1");
					return;
				}
			}
			if (!fl){
				puts("-1");
				return;
			}
			totb++;
			Order od = get_kth_data<Order>(totb, "order.txt");
			strcpy(od.id, tr);
			strcpy(od.S, S);
			strcpy(od.T, T);
			od.c = c;
			od.x = x;
			od.p = t.pri[fl] - t.pri[i];
			od.ds = d;
			od.dt = t.S + (de + t.dm[0][fl]);
			od.ps = i;
			od.pt = fl;
			od.d = de;
			od.s = t.lv[i];
			od.t = t.ar[fl];
			if (mn < x){
				od.ty = 1;
				puts("queue");
			}
			else{
				od.ty = 0;
                pr = train[k].r[de];
                for (int o = i; o < fl; o++){
					rt = get_kth_data<int>(pr + o, "res.txt") - x;
					add_to_file3(rt, pr + o, "res.txt");
				}
				printf("%lld\n", (LL)od.p * x);
				mn -= x;
			}

			tu = user[c];
			tu.cnt++;
			user.update_data(c, tu);
		//	user[c].cnt++;
			if (!user[c].fir){
				tu = user[c];
				tu.fir = tu.cur = totb;
				user.update_data(c, tu);
			//	user[c].fir = user[c].cur = totb;
			}
			else{
				tu = user[c];
				tu.fir = totb;
				user.update_data(c, tu);
			//	user[c].fir = totb;
				od.nxt = user[c].cur;

				tu = user[c];
				tu.cur = totb;
				user.update_data(c, tu);
			//	user[c].cur = totb;
			}
			add_to_file3(od,totb,"order.txt");
			if (!od.ty)
				return;
			if (!train[k].fir[de]){
				train[k].fir[de] = train[k].cur[de] = totb;
			}
			else{
				Order ood = get_kth_data<Order>(train[k].cur[de],"order.txt");
				ood.nxt_ = totb;
				add_to_file3(ood,train[k].cur[de],"order.txt");
				od.pre_ = train[k].cur[de];
				train[k].cur[de] = totb;
			}
			add_to_file3(od,totb,"order.txt");
			return;
		}
	}
	puts("-1");
}

void query_order(){
	char id[K1];
	Key k;
	scanf("%s%s", q, id);
	strcpy(k.id, id);

	if (!user.find(k) || !user[k].isLog){
		puts("-1");
		return;
	}
	printf("%d\n", user[k].cnt);
	for (int i = user[k].fir; i; /*???*/){
		Order ii = get_kth_data<Order>(i,"order.txt");
		if (ii.ty == 0)
			printf("[success] ");
		else
			if (ii.ty == 1)
				printf("[pending] ");
			else
				printf("[refunded] ");
		printf("%s %s ", ii.id, ii.S);
		ii.ds.output();
		printf(" ");
		ii.s.output();
		printf(" -> %s ", ii.T);
		ii.dt.output();
		printf(" ");
		ii.t.output();
		printf(" %d %d\n", ii.p, ii.x);
		i = ii.nxt;
	}
}

int refund_ticket(){
	char id[K1];
	Key k;
	int x(1);
	for (ch = getchar(); ch != '\n'; ch = getchar()){
		if (ch == '-'){
			ch = getchar();
			switch (ch){
				case 'n':
					scanf("%d", &x);
					break;
				case 'u':
					scanf("%s", id);
					break;
			}
		}
	}
	strcpy(k.id, id);

	if (!user.find(k) || !user[k].isLog)
		return -1;
	if (x < 1 || x > user[k].cnt)
		return -1;
	Key ts;
	int rt;
	for (int i = user[k].fir, j = 1; i; j++){
        Order ii = get_kth_data<Order>(i,"order.txt");
		if (j == x){
			if (ii.ty == 2)
				return -1;
			if (ii.ty){
				ii.ty = 2;
				add_to_file3(ii,i,"order.txt");
				return 0;
			}
			ii.ty = 2;
			strcpy(ts.id, ii.id);
			add_to_file3(ii,i,"order.txt");
            int pr = train[ts].r[ii.d];
            for (int j = ii.ps; j < ii.pt; j++){
				rt = get_kth_data<int>(pr + j, "res.txt") + ii.x;
				add_to_file3(rt, pr + j, "res.txt");
			}

			bool fl;
			for (int j = train[ts].fir[ii.d]; j; ){
				Order jj = get_kth_data<Order>(j,"order.txt");
				if (jj.ty != 1){
					if (!jj.pre_){
						train[ts].fir[ii.d] = jj.nxt_;
						Order od = get_kth_data<Order>(jj.nxt_,"order.txt");
						od.pre_ = 0;
						add_to_file3(od, jj.nxt_,"order.txt");
					}
					else{
						if (!jj.nxt_){
							Order od = get_kth_data<Order>(jj.pre_,"order.txt");
							od.nxt_ = 0;
							add_to_file3(od, jj.pre_,"order.txt");
						}
						else{
							Order od = get_kth_data<Order>(jj.pre_,"order.txt");
							Order ood = get_kth_data<Order>(jj.nxt_,"order.txt");
							od.nxt_ = jj.nxt_;
							ood.pre_ = jj.pre_;
							add_to_file3(od, jj.pre_,"order.txt");
							add_to_file3(ood, jj.nxt_,"order.txt");
						}
					}
					continue;
				}

				fl = 0;
                pr = train[ts].r[ii.d];
				for (int k = jj.ps; k < jj.pt; k++)
					if (get_kth_data<int>(pr + k, "res.txt") < jj.x){
						fl = 1;
						break;
					}
				if (fl)
					continue;

				for (int k = jj.ps; k < jj.pt; k++){
					rt = get_kth_data<int>(pr + k, "res.txt") - jj.x;
					add_to_file3(rt, pr + k, "res.txt");
				}
				jj.ty = 0;
				if (!jj.pre_){
					Order ood = get_kth_data<Order>(jj.nxt_,"order.txt");
					train[ts].fir[ii.d] = jj.nxt_;
					ood.pre_ = 0;
					add_to_file3(ood, jj.nxt_,"order.txt");
				}
				else{
					if (!jj.nxt_){
						Order od = get_kth_data<Order>(jj.pre_,"order.txt");
						od.nxt_ = 0;
						add_to_file3(od, jj.pre_,"order.txt");
					}
					else{
						string hhhhh = "order.txt";
						Order od = get_kth_data<Order>(jj.pre_,hhhhh);
						Order ood = get_kth_data<Order>(jj.nxt_,hhhhh);
						od.nxt_ = jj.nxt_;
						ood.pre_ = jj.pre_;
						add_to_file3(od, jj.pre_,"order.txt");
						add_to_file3(ood, jj.nxt_,"order.txt");
					}
				}
				add_to_file3(jj, j,"order.txt");
				j = jj.nxt_;
			}
			return 0;
        }
		i = ii.nxt;
    }
	return -1;
}

int clean(){
	user.clear();
	train.clear();
	sta.clear();
	return 0;
}

void exit(){
	for (int i = 1; i <= totl; i++){
		tl = get_kth_data<Log>(i, "lo.txt");
		tu = user[tl.k];
		tu.isLog = 0;
		user.update_data(tl.k, tu);
//		user[lo[i].k].isLog = 0;
	}		
	totl = 0;
	puts("bye");
}

int main(){
	char s[20];
	while (scanf("%s", s) != EOF){
		Tot++;
//		printf("%d\n", Tot);
//		cerr << Tot << endl;
		switch (s[0]){
			case 'a':
				if (s[4] == 'u')
					printf("%d\n", add_user());
				else
					printf("%d\n", add_train());
				break;

			case 'l':
				if (s[3] == 'i')
					printf("%d\n", Login());
				else
					printf("%d\n", Logout());
				break;

			case 'q':
				switch (s[10]){
					case 'i':
						query_profile();
						break;
					case 'n':
						query_train();
						break;
					case 'e':
						query_ticket();
						break;
					case 's':
						query_transfer();
						break;
					case 'r':
						query_order();
						break;
				}
				break;

			case 'r':
				if (s[2] == 'l')
					printf("%d\n", release_train());
				else
					printf("%d\n", refund_ticket());
				break;

			case 'm':
				modify_profile();
				break;
			
			case 'd':
				printf("%d\n", delete_train());
				break;
			
			case 'b':
				buy_ticket();
				break;
			
			case 'c':
				printf("%d\n", clean());
				break;

			case 'e':
				exit();
				break;
			//	return 0;
		}
	}

	return 0;
}
