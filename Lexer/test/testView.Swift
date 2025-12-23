//
//  CanvasView.swift
//  OODLab10
//
//  Created by Miroslav Aktuganov on 12.12.2025.
//

import SwiftUI

struct CanvasView: View {
    let shapes: [AnyShape]
    let selectedShapeId: UUID?
    let onSelectShape: (UUID?) -> Void
    let onShapeDragUpdate: (UUID, Point) -> Void
    let onShapeDragEnd: () -> Void
    let onResizeEnd: (Point, Size) -> Void

    var body: some View {
        ZStack(alignment: .topLeading) {
            ForEach(shapes) { anyShape in
                ShapeView(
                    shape: anyShape,
                    selectedShapeId: selectedShapeId,
                    onSelectShape: onSelectShape,
                    onShapeDragUpdate: onShapeDragUpdate,
                    onShapeDragEnd: onShapeDragEnd,
                    onResizeEnd: onResizeEnd
                )
            }
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(Color.gray.opacity(0.1))
        .contentShape(Rectangle())
        .onTapGesture {
            onSelectShape(nil)
        }
    }
}
