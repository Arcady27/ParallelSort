#include "batcher_sort.h"

void merge_batcher(vector<pair< int,int> >&schedule, vector<int>&first_group, vector<int>&second_group)
{
	int i, tacts = 0;
	int n0 = first_group.size(), n1 = second_group.size();
	
	if (n0 * n1 == 0)
		return;

	if (n0 * n1 == 1)
	{
		schedule.push_back(make_pair(first_group.front(), second_group.front()));
		return;
	}

	vector<int>first_group_odd, first_group_even, second_group_odd, second_group_even;
	for (int i = 0; i < n0; ++i)
	{
		if (i % 2 == 0)
			first_group_even.push_back(first_group[i]);
		else
			first_group_odd.push_back(first_group[i]);
	}

	for (int i = 0; i < n1; ++i)
	{
		if (i % 2 == 0)
			second_group_even.push_back(second_group[i]);
		else
			second_group_odd.push_back(second_group[i]);
	}

	merge_batcher(schedule, first_group_even, second_group_even);
	merge_batcher(schedule, first_group_odd, second_group_odd);
	
	for (int i = 1; i < n0 + n1 - 1; i += 2)
	{
		if (i < n0 - 1)
			schedule.push_back(make_pair(first_group[i], first_group[i + 1]));
		else if (i == n0 - 1)
			schedule.push_back(make_pair(first_group[i], second_group[0]));
		else
			schedule.push_back(make_pair(second_group[i - n0], second_group[i - n0 + 1]));

	}
}

void merge_batcher_parts(vector<pair<int, int> > &schedule, int first, int n0, int n1, int &comps)
{
	vector<int>first_group;
	vector<int>second_group;
	for (int i = first; i <= first + n0 + n1 - 1; ++i)
	{
		if (i < first + n0)
			first_group.push_back(i);
		else
			second_group.push_back(i);
	}
	
	merge_batcher(schedule, first_group, second_group);
}

void build_batcher_sort_net(vector<pair<int, int> > &schedule, int first, int last, int &comps)
{
	if (first == last)
		return;
	int m = (last - first + 1) / 2;
	build_batcher_sort_net(schedule, first, first + m - 1, comps);
	build_batcher_sort_net(schedule, first + m, last, comps);
	merge_batcher_parts(schedule, first, m, last - first + 1 - m, comps);
}

int calc_batcher_tacts(vector<pair<int, int> > &schedule)
{
	vector<set<int> >buckets;
	int i,j;
	for (i = 0; i < schedule.size(); ++i)
	{
		int first_comp = schedule[i].first, second_comp = schedule[i].second;
		bool inserted = false;
		for (j = buckets.size() - 1; j >= 0; --j)
		{
			if (buckets[j].find(first_comp) != buckets[j].end() || buckets[j].find(second_comp) != buckets[j].end())
				break;
		}
		if (!buckets.empty() && j + 1 <= buckets.size() - 1)
		{
			buckets[j + 1].insert(first_comp);
			buckets[j + 1].insert(second_comp);
		}
		else
		{
			set<int>new_bucket;
			new_bucket.insert(first_comp);
			new_bucket.insert(second_comp);
		
			buckets.push_back(new_bucket);
		}	
	}

	return buckets.size();
}