//
//  EditorModel.swift
//  OODLab10
//
//  Created by Miroslav Aktuganov on 13.12.2025.
//

import Foundation

class EditorModel {
    struct State {
        var shapes: [AnyShape]
    }

    private let shapeObservable: ShapeObservable
    private let shapeFactory: ShapeFactory
    private let documentManager = DocumentManager.shared
    private var commandHistory: CommandHistory
    private var state: State

    var documentToSave: ShapeDocument {
        ShapeDocument(shapes: state.shapes)
    }

    init() {
        self.state = .init(shapes: [])
        self.shapeObservable = .init()
        self.commandHistory = .init()
        self.shapeFactory = .init()
    }

    func createShape(params: CreateShapeParameters) {
        let shape = shapeFactory.createShape(parameters: params)
        let command = CreateShapeCommand(
            shape: shape,
            createShape: { [weak self] newShape in
                self?.state.shapes.append(newShape)
            },
            deleteShape: { [weak self] id in
                self?.state.shapes.removeAll(where: { $0.id == id })
            }
        )
        commandHistory.addAndExecute(command)
        shapeObservable.notifyRecievers(state: state)
    }

    func deleteShape(id: UUID) {
        guard let shape = state.shapes.first(where: { $0.id == id }) else { return }
        let command = DeleteShapeCommand(
            shape: shape,
            createShape: { [weak self] newShape in
                self?.state.shapes.append(newShape)
            },
            deleteShape: { [weak self] id in
                self?.state.shapes.removeAll(where: { $0.id == id })
            }
        )
        commandHistory.addAndExecute(command)
        shapeObservable.notifyRecievers(state: state)
    }

    func moveShape(id: UUID, position: Point) {
        guard let currentPosition = getShapePositionForMove(id: id) else { return }

        let command = MoveShapeCommand(
            prevPoint: currentPosition,
            newPoint: position,
            moveShape: { point in
                self.moveShape(id: id, to: point)
            }
        )
        commandHistory.addAndExecute(command)
        self.shapeObservable.notifyRecievers(state: self.state)
    }

    func resizeShape(id: UUID, position: Point, size: Size) {
        guard let oldState = getShapeState(id: id),
              let newState = calculateNewState(id: id, position: position, size: size) else { return }

        let command = ResizeShapeCommand(
            id: id,
            oldState: oldState,
            newState: newState,
            resizeShape: { (id, state) in
                self.resizeShape(id: id, to: state)
            }
        )
        commandHistory.addAndExecute(command)
        self.shapeObservable.notifyRecievers(state: self.state)
    }

    func setBorderColor(id: UUID, color: MyColor) {
        let command = SetBorderColorCommand(
            prevColor: state.shapes.first(where: { $0.id == id })?.shape.borderColor,
            newColor: color,
            setColor: { color in
                self.state.shapes = self.state.shapes.map {
                    if $0.id == id {
                        $0.shape.setBorderColor(color)
                        return $0
                    } else {
                        return $0
                    }
                }
                self.shapeObservable.notifyRecievers(state: self.state)
            }
        )
        commandHistory.addAndExecute(command)
    }

    func setBorderWidth(id: UUID, width: CGFloat) {
        let command = SetBorderWidthCommand(
            prevBorderWidth: state.shapes.first(where: { $0.id == id })?.shape.borderWidth ?? 0,
            newBorderWidth: width,
            setBorderWidth: { width in
                self.state.shapes = self.state.shapes.map {
                    if $0.id == id {
                        $0.shape.setBorderWidth(width)
                        return $0
                    } else {
                        return $0
                    }
                }
                self.shapeObservable.notifyRecievers(state: self.state)
            }
        )
        commandHistory.addAndExecute(command)
    }

    func setFillColor(id: UUID, color: MyColor) {
        let command = SetFillColorCommand(
            prevColor: state.shapes.first(where: { $0.id == id })?.shape.fillColor,
            newColor: color,
            setColor: { color in
                self.state.shapes = self.state.shapes.map {
                    if $0.id == id {
                        $0.shape.setFillColor(color)
                        return $0
                    } else {
                        return $0
                    }
                }
                self.shapeObservable.notifyRecievers(state: self.state)
            }
        )
        commandHistory.addAndExecute(command)
    }

    func undo() {
        commandHistory.undo()
        shapeObservable.notifyRecievers(state: state)
    }

    func redo() {
        commandHistory.redo()
        shapeObservable.notifyRecievers(state: state)
    }

    func addReciever(_ reciever: any ShapeNotifiable) {
        shapeObservable.addReciever(reciever)
    }

    func removeReciever(_ reciever: any ShapeNotifiable) {
        shapeObservable.removeReciever(reciever)
    }

    func load(from url: URL) throws {
        state.shapes = try documentManager.loadDocument(from: url)
        shapeObservable.notifyRecievers(state: state)
    }

    private func getShapePosition(id: UUID) -> Point? {
        for anyShape in state.shapes {
            if anyShape.id == id {
                switch anyShape {
                case .rectangle(let myRectangle):
                    return myRectangle.topleft
                case .ellipse(let myEllipse):
                    return myEllipse.center
                case .triangle(let myTriangle):
                    return myTriangle.center
                }
            }
        }
        return nil
    }

    private func getShapePositionForMove(id: UUID) -> Point? {
        for anyShape in state.shapes {
            if anyShape.id == id {
                switch anyShape {
                case .rectangle(let myRectangle):
                    return myRectangle.center
                case .ellipse(let myEllipse):
                    return myEllipse.center
                case .triangle(let myTriangle):
                    return myTriangle.center
                }
            }
        }
        return nil
    }

    private func getShapeSize(id: UUID) -> Size? {
        for anyShape in state.shapes {
            if anyShape.id == id {
                switch anyShape {
                case .rectangle(let myRectangle):
                    return Size(width: myRectangle.width, height: myRectangle.height)
                case .ellipse(let myEllipse):
                    return Size(width: myEllipse.width, height: myEllipse.height)
                case .triangle(let myTriangle):
                    return Size(width: myTriangle.width, height: myTriangle.height)
                }
            }
        }
        return nil
    }

    private func getShapeState(id: UUID) -> ShapeState? {
        guard let position = getShapePosition(id: id),
              let size = getShapeSize(id: id) else { return nil }
        return ShapeState(position: position, size: size)
    }

    private func calculateNewPosition(id: UUID, position: Point) -> Point? {
        for anyShape in state.shapes {
            if anyShape.id == id {
                switch anyShape {
                case .rectangle(let rect):
                    return Point(
                        x: position.x - rect.width / 2,
                        y: position.y - rect.height / 2
                    )
                case .ellipse:
                    return position
                case .triangle:
                    return position
                }
            }
        }
        return nil
    }

    private func calculateNewState(id: UUID, position: Point, size: Size) -> ShapeState? {
        guard let shape = state.shapes.first(where: { $0.id == id }) else { return nil }

        switch shape {
        case .rectangle:
            return ShapeState(
                position: Point(
                    x: position.x - size.width / 2,
                    y: position.y - size.height / 2
                ),
                size: size
            )
        case .ellipse:
            return ShapeState(position: position, size: size)
        case .triangle:
            return ShapeState(position: position, size: size)
        }
    }

    private func moveShape(id: UUID, to position: Point) {
        var updatedShapes = state.shapes

        for (index, anyShape) in updatedShapes.enumerated() {
            if anyShape.id == id {
                switch anyShape {
                case .rectangle(let rect):
                    let newRect = MyRectangle(
                        id: rect.id,
                        topleft: Point(
                            x: position.x - rect.width / 2,
                            y: position.y - rect.height / 2
                        ),
                        width: rect.width,
                        height: rect.height,
                        fillColor: rect.fillColor,
                        borderColor: rect.borderColor,
                        borderWidth: rect.borderWidth
                    )
                    updatedShapes[index] = .rectangle(newRect)

                case .ellipse(let ellipse):
                    let newEllipse = MyEllipse(
                        id: ellipse.id,
                        center: position,
                        width: ellipse.width,
                        height: ellipse.height,
                        fillColor: ellipse.fillColor,
                        borderColor: ellipse.borderColor,
                        borderWidth: ellipse.borderWidth
                    )
                    updatedShapes[index] = .ellipse(newEllipse)

                case .triangle(let triangle):
                    let currentCenter = triangle.center
                    let offsetX = position.x - currentCenter.x
                    let offsetY = position.y - currentCenter.y

                    let newVertices = triangle.vertices.map { vertex in
                        Point(x: vertex.x + offsetX, y: vertex.y + offsetY)
                    }

                    let newTriangle = MyTriangle(
                        id: triangle.id,
                        vertices: newVertices,
                        fillColor: triangle.fillColor,
                        borderColor: triangle.borderColor,
                        borderWidth: triangle.borderWidth
                    )
                    updatedShapes[index] = .triangle(newTriangle)
                }
                break
            }
        }
        state.shapes = updatedShapes
    }

    private func resizeShape(id: UUID, to state: ShapeState) {
        var updatedShapes = self.state.shapes

        for (index, anyShape) in updatedShapes.enumerated() {
            if anyShape.id == id {
                switch anyShape {
                case .rectangle(let rect):
                    let newRect = MyRectangle(
                        id: rect.id,
                        topleft: state.position,
                        width: state.size.width,
                        height: state.size.height,
                        fillColor: rect.fillColor,
                        borderColor: rect.borderColor,
                        borderWidth: rect.borderWidth
                    )
                    updatedShapes[index] = .rectangle(newRect)

                case .ellipse(let ellipse):
                    let newEllipse = MyEllipse(
                        id: ellipse.id,
                        center: state.position,
                        width: state.size.width,
                        height: state.size.height,
                        fillColor: ellipse.fillColor,
                        borderColor: ellipse.borderColor,
                        borderWidth: ellipse.borderWidth
                    )
                    updatedShapes[index] = .ellipse(newEllipse)

                case .triangle(let triangle):
                    let oldCenter = triangle.center
                    let scaleX = state.size.width / max(triangle.width, 1)
                    let scaleY = state.size.height / max(triangle.height, 1)

                    let newVertices = triangle.vertices.map { v in
                        Point(
                            x: (v.x - oldCenter.x) * scaleX + state.position.x,
                            y: (v.y - oldCenter.y) * scaleY + state.position.y
                        )
                    }

                    let newTriangle = MyTriangle(
                        id: triangle.id,
                        vertices: newVertices,
                        fillColor: triangle.fillColor,
                        borderColor: triangle.borderColor,
                        borderWidth: triangle.borderWidth
                    )
                    updatedShapes[index] = .triangle(newTriangle)
                }
                break
            }
        }
        self.state.shapes = updatedShapes
    }
}

struct ShapeState {
    let position: Point
    let size: Size
}
