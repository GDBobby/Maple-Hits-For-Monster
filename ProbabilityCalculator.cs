						//monsterName,    hits, probability
public static Dictionary<string, Dictionary<int, float>> expectedHitsToKill = new Dictionary<string, Dictionary<int, float>>();
public static void GetExpectedHitsToKill(string monsterName, int levelDiff, int minDmg, int maxDmg, int targetHP, float skillPercentage = 100.0f, int skillLines = 1)
{
	if(levelDiff < 0)
	{
		levelDiff = 0;
	}

	float fMinDmg = (minDmg * skillPercentage / 100.0f) * (1.0f - 0.01f * levelDiff);
	float fMaxDmg = (maxDmg * skillPercentage / 100.0f) * (1.0f - 0.01f * levelDiff);
	minDmg = (int)Math.Floor(fMinDmg);
	maxDmg = (int)Math.Floor(fMaxDmg);

	float averageDmg = (minDmg + maxDmg) / 2.0f * skillPercentage / 100.0f;
	float variance;
	{
		int dmgRange = maxDmg - minDmg + 1; //possible rolls
		variance = (dmgRange * dmgRange - 1.0f) / 12.0f;
	}
	int averageHitsToKill = (int)Math.Ceiling(targetHP / averageDmg);

	//checking lower end, setting the starting point from 1% or more likely to take more hits to kill
	int hitsToKill = averageHitsToKill - 1;
	float cumulative_probability = 0.0f;
	while (true)
	{

		float meanSum = hitsToKill * averageDmg;
		float stdDev = (float)Math.Sqrt(meanSum * variance);

		float z_score = (targetHP - meanSum) / stdDev;
		cumulative_probability = 1.0f - 0.5f * (float)Erfc(-z_score / Math.Sqrt(2.0));

		if (cumulative_probability < 0.01f)
		{
			break;
		}
		hitsToKill--;
	};

	Dictionary<int, float> ret = new Dictionary<int, float>();

	//iterating through each hit count until its 99% likely the monster is dead
	while (cumulative_probability < 0.99f)
	{
		float prev_cumulative_probability = cumulative_probability;
		hitsToKill++;
		float meanSum = hitsToKill * averageDmg;
		float stdDev = (float)Math.Sqrt(meanSum * variance);

		float z_score = (targetHP - meanSum) / stdDev;
		cumulative_probability = 1.0f - 0.5f * (float)Erfc(-z_score / Math.Sqrt(2.0));

		float cum_diff = cumulative_probability - prev_cumulative_probability;
		if (cum_diff < 0.005f)
		{
			break;
		}
		
		ret.Add(hitsToKill, cum_diff * 100.0f);
	}

	//filtering the hit count into skill hits
	if (skillLines > 1)
	{
		Dictionary<int, float> filteredRet = new Dictionary<int, float>();
		//merge probabilities from condensed hitCount of skill lines
		foreach (var hitCount in ret)
		{
			int skillHitCount = (int)Math.Ceiling((float)hitCount.Key / (float)skillLines);
			if (filteredRet.ContainsKey(skillHitCount))
			{
				filteredRet[skillHitCount] += hitCount.Value;
			}
			else
			{
				filteredRet.Add(skillHitCount, hitCount.Value);
			}
		}
		expectedHitsToKill.Add(monsterName, filteredRet);
	}
	else
	{
		expectedHitsToKill.Add(monsterName, ret);
	}
}