#include "cooperative scheduler.h"

sTask SCH_Tasks_G[SCH_MAX_TASKS];
uint8_t current_index_task = 0;

void SCH_Init(void){
	current_index_task = 0;
}

void SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	if (current_index_task < SCH_MAX_TASKS){
		SCH_Tasks_G[current_index_task].pTask = pFunction;				// con trỏ hàm trỏ đến hàm thực hiện tác vụ này
		SCH_Tasks_G[current_index_task].Delay = DELAY;					// gán giá trị delay
		SCH_Tasks_G[current_index_task].Period = PERIOD;				// gán giá trị period
		SCH_Tasks_G[current_index_task].RunMe = 0;						// tác vụ mới thêm vào hàng nên chưa thể ngay lập tức thực thi

		SCH_Tasks_G[current_index_task].TaskID = current_index_task++;	// ID của tác vụ là vị trí trong hàng chờ
	}
}

void SCH_Update(void){
	for (int i = 0; i <  current_index_task; i ++){
		if (SCH_Tasks_G[i].Delay > 0){									// nếu delay chưa bằng 0 thì tiếp tục chờ
			SCH_Tasks_G[i].Delay--;
		}
		else{															// nếu delay = 0, tức hết thời gian chờ, cờ thực thi được bật
			SCH_Tasks_G[i].Delay = SCH_Tasks_G[i].Period;
			SCH_Tasks_G[i].RunMe++;
		}
	}
}

void SCH_Dispatch_Tasks(void){
	for (int i = 0; i < current_index_task; i++){
		if (SCH_Tasks_G[i].RunMe > 0){									// nếu cờ được bật, thực thi tác vụ
			SCH_Tasks_G[i].RunMe--;
			(*SCH_Tasks_G[i].pTask)();

			if (SCH_Tasks_G[i].Period == 0){							// nếu period = 0, tác vụ chỉ thực hiện 1 lần duy nhất
				SCH_Delete(i);											// trước khi bị xóa khỏi hàng chờ
			}
		}
	}
}

void SCH_Delete(uint32_t ID){
	if (ID < 0 || ID > current_index_task){								// ID của tác vụ không hợp lệ
		return;
	}
	else if (ID == current_index_task){									// nếu tác vụ cần xóa là tác vụ hiện tại, cập nhật các thuộc tính của tác vụ này
		SCH_Tasks_G[current_index_task].pTask = 0x0000;
		SCH_Tasks_G[current_index_task].Delay = 0;
		SCH_Tasks_G[current_index_task].Period = 0;
		SCH_Tasks_G[current_index_task].RunMe = 0;

		SCH_Tasks_G[current_index_task].TaskID = 0;
	}
	else if (ID < current_index_task){									// nếu tác vụ không phải là tác vụ hiện tại, di chuyển các tác vụ sau lên trước khi xóa tác vụ cuối cùng
		for (int i = ID; i < current_index_task; i++){
			SCH_Tasks_G[i].pTask = SCH_Tasks_G[i + 1].pTask;
			SCH_Tasks_G[i].Delay = SCH_Tasks_G[i + 1].Delay;
			SCH_Tasks_G[i].Period = SCH_Tasks_G[i + 1].Period;
			SCH_Tasks_G[i].RunMe = SCH_Tasks_G[i + 1].RunMe;

			SCH_Tasks_G[i].TaskID = SCH_Tasks_G[i+1].TaskID;
		}
		SCH_Tasks_G[current_index_task].pTask = 0x0000;
		SCH_Tasks_G[current_index_task].Delay = 0;
		SCH_Tasks_G[current_index_task].Period = 0;
		SCH_Tasks_G[current_index_task].RunMe = 0;

		SCH_Tasks_G[current_index_task].TaskID = 0;

		current_index_task--;
	}
}

