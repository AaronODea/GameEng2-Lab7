#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable> 
#include <queue>

std::mutex m_mutex;
std::queue<int> m_queue;
std::condition_variable m_conditional_Variable;

bool cookingDone = false;//if cooking done start eating 


void producer(int Buffer)
{
	std::cout << "Producer Starting" << std::endl;

	for (int i = 0; i < Buffer; ++i)
	{		
		std::lock_guard<std::mutex> m_unique_lock(m_mutex);
		m_queue.push(i);

		std::cout << "Making a Burger :" << i << std::endl;
		m_conditional_Variable.notify_all();
	}
	{
		std::cout << "Finished Making Burgers" << std::endl;
		std::cout << std::endl;
		std::lock_guard<std::mutex> m_unique_lock(m_mutex);
		cookingDone = true;
	}
	m_conditional_Variable.notify_all();
}

void consumer()
{
	std::cout << "Consumer Starting" << std::endl;

	while (true) 
	{
		std::unique_lock<std::mutex> m_unique_lock(m_mutex);
		m_conditional_Variable.wait(m_unique_lock, [] { return cookingDone || !m_queue.empty(); });

		while (!m_queue.empty())
		{
			std::cout << "Eating Burger:" << m_queue.front() << std::endl;
			m_queue.pop();
		}
		if (cookingDone)
		{
			std::cout << "Finished Eating Burgers" << std::endl;
			std::cout << std::endl;
			break;
		}
	}
}

int main() {
	
	std::thread secondThread(consumer);
	std::thread firstThread(producer, 40);
	std::cout << std::endl;

	firstThread.join();
	secondThread.join();

	std::cout << "finished!" << std::endl;
	system("pause");
}