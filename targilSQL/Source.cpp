#include "Header.h"

int main()
{
	vector<table*> tableArr = scanTxtFile();
	int queryResult = scanQuery(tableArr);
}

vector<string> getQuery()
{
	string input;
	cout << "enter an sql query" << endl;
	getline(cin, input);

	vector<string> result;
	istringstream iss(input);
	for (string s; iss >> s; )
		result.push_back(s);

	return result;
}

int scanQuery(vector<table*> i_tableArr)
{
	vector<string> query = getQuery();
	int result = 0;
	int i = 0;

	int selectIndex = findString(query, "SELECT");
	int distinctIndex = findString(query, "DISTINCT");
	int fromIndex = findString(query, "FROM");
	int whereIndex = findString(query, "WHERE");
	int endIndex = findString(query, ";");

	if (selectIndex == -1 || fromIndex == -1 || whereIndex == -1)
	{
		result = 1;
	}
	if (!(selectIndex < fromIndex < whereIndex))
		result = 1;
	if (!(selectIndex == 0))
		result = 1;

	if (distinctIndex == 1)
	{
		i++;
	}
	else if (distinctIndex != -1)
	{
		result = 1; //distinct is not in valid place
	}

	if (IsAttributeList(i_tableArr, query, selectIndex, fromIndex, distinctIndex) == false)
	{
		result = 2;
	}
	if (IsTableList(i_tableArr, query, fromIndex, whereIndex) == false)
		result = 3;
	if (IsCondition(i_tableArr, query, whereIndex, endIndex) == false)
		result = 4;

	return result;
}

string removeSpaces(vector<string> arr, int beginIndex, int endIndex)
{
	string result = "";
	for (int i = beginIndex; i < endIndex; i++)
	{
		result = result + arr[i];
	}

	return result;
}

int IsCondition(vector<table*> tableArr, vector<string>arr, int whereIndex, int endIndex)
{

}

int IsTableList(vector<table*> tableArr, vector<string>arr, int fromIndex, int whereIndex)
{
	string tablePart = "";
	tablePart = removeSpaces(arr, fromIndex + 1, whereIndex);

	bool checkAll = true;
	bool checkOne = false;
	int pos, dotPos;
	string currentTable;
	int flag = 0;
	pos = tablePart.find(",");
	while (pos > 0 || flag == 0)
	{
		pos = tablePart.find(",");
		if (pos > 0)
		{
			currentTable = tablePart.substr(0, pos);
			tablePart = tablePart.substr(pos + 1);
		}
		else //last attribute
		{
			currentTable = tablePart;
		}

		for (int i = 0; i < tableArr.capacity(); i++)
		{
			if (tableArr[i]->tableName == currentTable)
			{
				checkOne = true;
				break;
			}
		}
		checkAll = checkAll && checkOne;
		checkOne = false;
		if (pos < 0)
		{
			flag = 1;
		}
	}


	return checkAll;
}

int IsAttributeList(vector<table*> tableArr, vector<string> arr, int selectIndex, int fromIndex, int distinctIndex)
{
	string attributePart = "";
	if (distinctIndex == -1)
		attributePart = removeSpaces(arr, selectIndex + 1, fromIndex);
	else
		attributePart = removeSpaces(arr, distinctIndex + 1, fromIndex);


	return isAsterisk(arr, fromIndex, distinctIndex) || isAttList(attributePart, tableArr);
}

bool isAttList(string attributePart, vector<table*> tableArr)
{
	bool checkAll = true;
	bool checkOne = false;
	int pos, dotPos;
	string currentAttribute, tableName, attributeName;
	int flag = 0;
	pos = attributePart.find(",");
	while (pos > 0 || flag == 0)
	{
		pos = attributePart.find(",");
		if (pos > 0)
		{
			currentAttribute = attributePart.substr(0, pos);
			attributePart = attributePart.substr(pos + 1);
		}
		else //last attribute
		{
			currentAttribute = attributePart;
		}

		dotPos = currentAttribute.find(".");
		if (dotPos != -1) //check if found "."
		{
			tableName = currentAttribute.substr(0, dotPos);
			attributeName = currentAttribute.substr(dotPos + 1);
			attribute* curr = nullptr;
			for (int i = 0; i < tableArr.capacity(); i++)
			{
				if (tableArr[i]->tableName == tableName)
				{
					curr = tableArr[i]->m_attributeList->head;
					while (curr != nullptr)
					{
						if (curr->name == attributeName)
						{
							checkOne = true;
							break;
						}
						curr = curr->next;
					}
				}
				if (checkOne == true)
					break;
			}
		}
		checkAll = checkAll && checkOne;
		checkOne = false;
		if (pos < 0)
		{
			flag = 1;

		}
	}

	return checkAll;
}

bool isAsterisk(vector<string> arr, int fromIndex, int distinctIndex)
{
	bool valid = false;
	if (distinctIndex == 1 && fromIndex == 3 && arr[2] == "*")
	{//"DISTINCT" is exist
		valid = true;
	}
	if (distinctIndex == -1 && fromIndex == 2 && arr[1] == "*")
	{//"DISTINCT" is not exist
		valid = true;
	}

	return valid;
}

int findString(vector<string> arr, string check)
{
	int result = -1;
	for (int i = 0; i < arr.capacity(); i++)
	{
		if (arr[i] == check)
		{
			result = i;
			break;
		}
	}

	return result;
}

vector<table*> scanTxtFile()
{
	vector<table*> tableArr;
	table* newTable;

	string line;
	ifstream myfile;
	myfile.open("scheme.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			line.erase(remove(line.begin(), line.end(), ' '), line.end());

			newTable = new table();
			newTable->m_attributeList = new attributeList();
			makeEmptyAttributeList(newTable->m_attributeList);


			int pos = line.find("(");
			newTable->tableName = line.substr(0, pos); //find table name

			line = line.substr(pos + 1);
			pos = line.find(",");
			string newAttribute;
			int typePos;
			attribute* newAtt;
			int flag = 0;
			while (pos > 0 || flag == 0) //scan attributes
			{
				if (line.find(",") == -1)
				{
					flag = 1;
				}
				newAtt = new attribute();
				newAttribute = line.substr(0, pos);
				typePos = newAttribute.find(":");
				newAtt->name = newAttribute.substr(0, typePos);
				newAtt->type = newAttribute.substr(typePos + 1);
				newAtt->next = nullptr;
				insertNodeToEndAttributeList(newTable->m_attributeList, newAtt);
				newAtt = nullptr;
				line = line.substr(pos + 1);
				pos = line.find(",");
			}
			newTable->m_attributeList->tail->type = (newTable->m_attributeList->tail->type).substr(0, newTable->m_attributeList->tail->type.length() - 1);
			tableArr.push_back(newTable);
			newTable = nullptr;
		}
		myfile.close();
	}

	return tableArr;
}

void makeEmptyAttributeList(attributeList* l)
{
	l->head = l->tail = NULL;
}


void insertNodeToEndAttributeList(attributeList *lst, attribute *tail)
{
	if (lst->head == NULL)
	{
		lst->head = tail;
		lst->tail = tail;
	}
	else
	{
		lst->tail->next = tail;
		lst->tail = tail;
	}
}

