#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include<map>
#include<vector>
#include<sstream>
using namespace std;


void naiveCopy(string inputFilename, string outputFilename) {
  ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
  ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
  char c;
  while (ifs.get(c)) ofs.put(c);
  ofs.close();
  ifs.close();
}


struct node /*the node of huffman tree*/
{
	int f = 0;/*frequence of the char*/
	char c;
	node* left = NULL;
	node* right = NULL;
	node* parent = NULL;


};
char stoc(string tmp)/*string to the corresponding char*/
{
	int n = 1, sum = 0;
	for (int i = 7; i >= 0; --i)
	{
		sum += n * (tmp[i] - '0');
		n *= 2;
	}
	return char(sum);
}
string ctos(char tmp)/*char to the correspoding string*/
{
	int tmpn = int(tmp);
	if (tmpn < 0)
	{
		tmpn += 256;
	}
	string tmpc;
	string key;
	for (int i = 0; i < 8; ++i)
	{
		char c = '0' + tmpn % 2;
		tmpc.push_back(c);
		tmpn /= 2;
		key = tmpc + key;
		tmpc.clear();
	}
	return key;

}
char itoc(int tmp)
{
	return char(tmp);
}
int ctoi(char tmp)
{
	int tmpn = int(tmp);
	if (tmpn < 0)/*the mistake between char and unsigned char*/
	{
		tmpn += 256;
	}
	return tmpn;
}
void bianli(node* a, map<char, string> &data, string road)/*read the huffman tree and generate the huffman code*/
{

	if (a->left == NULL && a->right == NULL)
	{
		data[a->c] = road;
	}
	else
	{
		if (a->left != NULL)
		{


			bianli(a->left, data, road + "0");

		}
		if (a->right != NULL)
		{

			bianli(a->right, data, road + "1");

		}
	}
}
void generate_huffmantree(map<char,int>&fre,vector<node> &tree)
{
		int n = fre.size();	
		map<char, int>::iterator it = fre.begin();
		for (int i = 0; i < n; ++i)
		{
			tree[i].f = it->second;
			tree[i].c = it->first;

			++it;
		}

		int min1, min2;/*find the min frequence*/
		for (int i = n; i < 2 * n - 1; ++i)
		{
			min1 = -1;
			min2 = -1;
			for (int j = 0; j < i; ++j)
			{
				if (tree[j].parent == NULL)
				{
					if (min1 == -1)
					{
						min1 = j;
					}
					else if (tree[min1].f > tree[j].f)
					{
						min1 = j;
					}
				}
			}
			for (int j = 0; j < i; ++j)
			{
				if (tree[j].parent == NULL)
				{
					if (min2 == -1)
					{
						if (min1 != j)
							min2 = j;
					}
					else if (tree[min2].f > tree[j].f&&min1 != j)
					{

						min2 = j;

					}

				}


			}
			tree[i].f = tree[min1].f + tree[min2].f;
			tree[i].left = &tree[min1];
			tree[i].right = &tree[min2];
			tree[min1].parent = &tree[i];
			tree[min2].parent = &tree[i];
		}
	

}
void compress(string inputFilename, string outputFilename)
{
	ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
	ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
	map<char, int> fre;
	char c;

	while (ifs.get(c))/*read the whole file and generate a tree based on char and frequence*/
	{

		if (fre.count(c) == 0)
		{
			fre[c] = 1;
		}
		else
		{
			++fre[c];
		}
	}
	int n = fre.size();
	if (n != 0)/*generate the huffman tree*/
	{
		
		vector<node> tree(2 * n - 1);
		generate_huffmantree(fre,tree);
		
		map<char, string> data;
		string road;
		bianli(&tree[2 * n - 2], data, road);/*generate the huffman code*/
		int x = 0;
		int sum = tree[2 * n - 2].f;
		ofs << sum << '\n';		
		ofs << n << '\n';		
		
		map<char, string>::iterator it1 = data.begin();
		while (it1 != data.end())/*compress the huffman informantion into a file*/
		{

			ofs << it1->first ;
			string tmp=it1->second;
			ofs<<itoc(tmp.length());
			string output;
			int position=0;
			for (int i = 0; i < tmp.size(); ++i)
			{
				if (position < 8)
				{
					output = output + tmp[i];
					++position;
				}
				else
				{

					char tmpc = stoc(output);
					ofs << tmpc;
					output.clear();
					output = output + tmp[i];
					position = 1;
				}

			}


		
			while (position < 8)
			{
				output = output + '0';
				++position;
			}
			ofs << stoc(output);
			++it1;
		}
		ifs.clear();/*reread the input file*/
		ifs.seekg(0);
		string output;
		string cache, tmp;
		int position = 0;
		while (ifs.get(c))/*compress the file */
		{

			tmp = data[c];
			for (int i = 0; i < tmp.size(); ++i)
			{
				if (position < 8)
				{
					output = output + tmp[i];
					++position;
				}
				else
				{

					char tmpc = stoc(output);
					ofs << tmpc;
					output.clear();
					output = output + tmp[i];
					position = 1;
				}

			}


		}
		while (position < 8)
		{
			output = output + '0';
			++position;
		}
		ofs << stoc(output);
	}
}

void decompress(string inputFilename, string outputFilename) {
	ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
	ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
	char c = '0', tmp = '0';
	string n;

	if (ifs.get(c))/*read the huffman information*/
	{
		int sum;/*read the total number of char*/
		while (c != '\n')
		{
			n = n + c;
			ifs.get(c);

		}
		stringstream ss;
		ss << n;
		ss >> sum;		
		
		n.clear();
		ifs.get(c);/*read the num of huffman tree node*/
		while (c != '\n')
		{
			n = n + c;
			ifs.get(c);
		}
		stringstream aa;
		aa << n;
		int num;
		aa >> num;
		cout<<num;
		n.clear();
		
		map<string, char> data;
		for (int i = 0; i < num; ++i)
		{
			ifs.get(c);
			ifs.get(tmp);			
			int size=ctoi(tmp);			
			int tmpsize=size;
			char tmpc;
			while (tmpsize>0)
			{
				ifs.get(tmpc);
				n=n+ctos(tmpc);
				tmpsize-=8;
			}
			n=n.substr(0,size);
			data[n] = c;
			cout<<n;
			n.clear();

		}

		string now, key;
		int position = 0;
		ifs.get(c);
		now = ctos(c);
		while (ifs)/*decompress the content*/
		{

			while (data.count(key) == 0)
			{
				if (position < 8)
				{
					key += now[position];
					position++;
				}
				else
				{
					if (ifs)
					{
						ifs.get(c);
						now = ctos(c);
						position = 0;
					}
					else
					{
						break;
					}
				}
			}
			if (!ifs)
			{
				break;
			}
			ofs.put(data[key]);
			key.clear();
			--sum;
			if (sum == 0) break;
		}
	}
}

void usage(string prog) {
  cerr << "Usage: " << endl
      << "    " << prog << "[-d] input_file output_file" << endl;
  exit(2);
}

int main(int argc, char* argv[]) {
  int i;
  string inputFilename, outputFilename;
  bool isDecompress = false;
  for (i = 1; i < argc; i++) {
    if (argv[i] == string("-d")) isDecompress = true;
    else if (inputFilename == "") inputFilename = argv[i];
    else if (outputFilename == "") outputFilename = argv[i];
    else usage(argv[0]);
  }
  if (outputFilename == "") usage(argv[0]);
  if (isDecompress) decompress(inputFilename, outputFilename);
  else compress(inputFilename, outputFilename);
  return 0;
	
}
