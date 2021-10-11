#define TOP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

namespace Bamtang
{
	class Maze
	{

	private:

		char* board;
		unsigned int n;

	public:

		Maze(const unsigned int& n)
		{
			srand((unsigned)time(NULL));

			CreateMaze2D(n);
		}

	private:

		~Maze()
		{
			board = NULL;
			delete board;
			delete this;
		}

		void Initialize()
		{
			for (int i = 0; i < n * n; i++)
			{
				board[i] = '*';
			}
		}

		int Index(int x, int y)
		{
			return y * n + x;
		}

		int CheckBoard(int x, int y)
		{
			if (x < 0 || x >= n) return false;
			if (y < 0 || y >= n) return false;
			return true;
		}

		void Draw()
		{
			for (int y = 0; y < n; ++y)
			{
				for (int x = 0; x < n; ++x)
				{
					std::cout << board[Index(x, y)];
				}
				std::cout << "\n";
			}
		}

		void GenerateMaze(int x, int y)
		{
			board[Index(x, y)] = ' ';

			int direction[4];
			direction[0] = TOP;
			direction[1] = RIGHT;
			direction[2] = DOWN;
			direction[3] = LEFT;

			for (int i = 0; i < 4; ++i)
			{
				int random = rand() % 4;
				int temp = direction[random];
				direction[random] = direction[i];
				direction[i] = temp;
			}

			for (int i = 0; i < 4; ++i)
			{
				int dx = 0, dy = 0;
				switch (direction[i])
				{
				case TOP: dy = -1; break;
				case DOWN: dy = 1; break;
				case RIGHT: dx = 1; break;
				case LEFT: dx = -1; break;
				}

				int x2 = x + (dx << 1);
				int y2 = y + (dy << 1);

				if (CheckBoard(x2, y2) && board[Index(x2, y2)] == '*')
				{
					board[Index(x2 - dx, y2 - dy)] = ' ';
					GenerateMaze(x2, y2);
				}
			}
		}

		void FindPathBFS()
		{
			std::queue<std::pair<int, int>> _queue;
			std::pair<int, int> current;

			_queue.push({ 1,1 });
			board[Index(1, 1)] = 'A';

			bool* visited = new bool[(n * n)]();
			for (int i = 0; i < n * n; i++) visited[i] = false;

			const int dx[4] = { 0, 0, 1, -1 };
			const int dy[4] = { 1, -1, 0, 0 };

			while (!_queue.empty())
			{
				current = _queue.front();
				_queue.pop();

				visited[Index(current.first, current.second)] = true;
				for (int i = 0; i < 4; ++i)
				{
					int nx = dx[i] + current.first;
					int ny = dy[i] + current.second;

					if (nx >= 0 && nx < n && ny >= 0 && ny < n && visited[Index(nx, ny)] == false && board[Index(nx, ny)] != '*')
					{
						_queue.push({ nx, ny });
					}
				}
			}

			board[Index(current.first, current.second)] = 'B';

		}

		void CreateMaze2D(int n)
		{
			if (n % 2 == 0) n++;
			this->n = n;

			delete board;
			board = NULL;
			board = new char[(n * n)]();

			Initialize();
			GenerateMaze(1, 1);
			FindPathBFS();
			Draw();
		}
	};

}





