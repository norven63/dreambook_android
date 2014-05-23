#pragma once

/*
	在CRenderDoc（可编辑文档）与CRichBook之间互相转换
*/

void FromParaStream2RenderDoc(CRichBook& ps,CRenderDoc& doc);
void FromRenderDoc2ParaStream(CRenderDoc& doc,CRichBook& book);