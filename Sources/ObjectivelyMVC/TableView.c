/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>

#include <ObjectivelyMVC/TableView.h>

#define _Class _TableView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	TableView *this = (TableView *) self;

	release(this->columns);
	release(this->rows);
	release(this->headerView);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	const SDL_Rect bounds = $(self, bounds);

	TableView *this = (TableView *) self;

	View *headerView = (View *) this->headerView;
	View *scrollView = (View *) this->scrollView;

	if (headerView->hidden == false) {
		scrollView->frame.y = headerView->frame.h;
		scrollView->frame.h = bounds.h - headerView->frame.h;
	} else {
		scrollView->frame.y = 0;
		scrollView->frame.h = bounds.h;
	}

	const Array *rows = (Array *) this->rows;
	for (size_t i = 0; i < rows->count; i++) {

		TableRowView *row = (TableRowView *) $(rows, objectAtIndex, i);
		row->stackView.view.frame.h = this->rowHeight;

		if (this->usesAlternateBackgroundColor && (i & 1)) {
			row->stackView.view.backgroundColor = this->alternateBackgroundColor;
		}
	}

	super(View, self, layoutSubviews);

	$((View *) this->contentView, sizeToFit);
}

#pragma mark - TableView

/**
 * @fn void TableView::addColumn(TableView *self, TableColumn *column)
 *
 * @memberof TableView
 */
static void addColumn(TableView *self, TableColumn *column) {

	assert(column);

	$(self->columns, addObject, column);
}

/**
 * @fn TableColumn *TableView::columnWithIdentifier(const TableView *self)
 *
 * @memberof TableView
 */
static TableColumn *columnWithIdentifier(const TableView *self, const char *identifier) {

	assert(identifier);

	const Array *columns = (Array *) self->columns;
	for (size_t i = 0; i < columns->count; i++) {

		TableColumn *column = $(columns, objectAtIndex, i);
		if (strcmp(identifier, column->identifier) == 0) {
			return column;
		}
	}

	return NULL;
}

/**
 * @fn TableView *TableView::initWithFrame(TableView *self, const SDL_Rect *frame)
 *
 * @memberof TableView
 */
static TableView *initWithFrame(TableView *self, const SDL_Rect *frame) {
	
	self = (TableView *) super(View, self, initWithFrame, frame);
	if (self) {
		self->columns = $$(MutableArray, array);
		assert(self->columns);

		self->rows = $$(MutableArray, array);
		assert(self->rows);

		self->headerView = $(alloc(TableHeaderView), initWithTableView, self);
		assert(self->headerView);

		$((View *) self, addSubview, (View *) self->headerView);

		self->contentView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->contentView);

		self->scrollView = $(alloc(ScrollView), initWithFrame, NULL);
		assert(self->scrollView);

		$(self->scrollView, setContentView, (View *) self->contentView);

		$((View *) self, addSubview, (View *) self->scrollView);

		self->alternateBackgroundColor = Colors.AlternateColor;
		self->usesAlternateBackgroundColor = true;

		self->cellSpacing = DEFAULT_TABLE_VIEW_CELL_SPACING;
		self->rowHeight = DEFAULT_TABLE_VIEW_ROW_HEIGHT;

		self->view.backgroundColor = Colors.DefaultColor;
	}
	
	return self;
}

/**
 * @brief ArrayEnumerator to remove TableRowViews from the table's contentView.
 */
static _Bool reloadData_removeRows(const Array *array, ident obj, ident data) {
	$((View *) obj, removeFromSuperview); return false;
}

/**
 * @fn void TableView::reloadData(TableView *self)
 *
 * @memberof TableView
 */
static void reloadData(TableView *self) {

	$((Array *) self->rows, enumerateObjects, reloadData_removeRows, NULL);
	$(self->rows, removeAllObjects);

	TableRowView *headerView = (TableRowView *) self->headerView;
	$(headerView, removeAllCells);

	const Array *columns = (Array *) self->columns;
	for (size_t i = 0; i < columns->count; i++) {

		const TableColumn *column = $(columns, objectAtIndex, i);
		$(headerView, addCell, (TableCellView *) column->headerCell);
	}

	const size_t numberOfRows = self->dataSource.numberOfRows(self);
	for (size_t i = 0; i < numberOfRows; i++) {

		TableRowView *row = $(alloc(TableRowView), initWithTableView, self);
		assert(row);

		$(self->rows, addObject, row);
		$((View *) self->contentView, addSubview, (View *) row);

		for (size_t j = 0; j < columns->count; j++) {

			const TableColumn *column = $(columns, objectAtIndex, j);

			TableCellView *cell = self->delegate.cellForColumnAndRow(self, column, i);
			assert(cell);

			cell->value = self->dataSource.valueForColumnAndRow(self, column, i);

			$(row, addCell, cell);
		}
	}

	self->selectedRow = -1;
}

/**
 * @fn void TableView::removeColumn(TableView *self, TableColumn *column)
 *
 * @memberof TableView
 */
static void removeColumn(TableView *self, TableColumn *column) {

	assert(column);

	$(self->columns, removeObject, column);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;

	((TableViewInterface *) clazz->interface)->addColumn = addColumn;
	((TableViewInterface *) clazz->interface)->columnWithIdentifier = columnWithIdentifier;
	((TableViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((TableViewInterface *) clazz->interface)->reloadData = reloadData;
	((TableViewInterface *) clazz->interface)->removeColumn = removeColumn;
}

Class _TableView = {
	.name = "TableView",
	.superclass = &_StackView,
	.instanceSize = sizeof(TableView),
	.interfaceOffset = offsetof(TableView, interface),
	.interfaceSize = sizeof(TableViewInterface),
	.initialize = initialize,
};

#undef _Class
