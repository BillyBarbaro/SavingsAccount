#include "main.h"

customer create_customer(int withdrawl_amount) {
	new_customer = malloc(sizeof(customer));
	new_customer->withdrawl_amount = withdrawl_amount;

	return new_customer
}

node create_node(int customer) {
	new_node = malloc(sizeof(node));
	new_node->value = customer;

	return new_node;
}

void add_customer_to_queue(linked_list queue, int withdrawl_amount) {
	customer *new_customer = create_customer(withdrawl_amount);
	node *new_node = create_node(new_customer);
	new_node->next = queue->tail;
	queue->tail = new_node;
}

void serve_first_in_queue(linked_list queue) {
	node *current_head = queue->head
	node *new_head = current_head->next;
	queue->head = new_first
}

int first_customer_amount(linked_list queue) {
	customer *first_customer = queue->head;
	return first_customer->withdrawl_amount;
}