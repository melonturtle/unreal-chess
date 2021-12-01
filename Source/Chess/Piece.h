// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessInfo.h"
#include "ChessUtil.h"
#include "Piece.generated.h"


UCLASS()
class CHESS_API APiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APiece();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const EPieceColor GetPieceColor() const { return PieceColor; }
	const EPieceType GetPieceType() const { return PieceType; }
	class UStaticMeshComponent* GetStaticMeshComponent();

	virtual void UpdateBasicMoves();
	virtual TSet<FVector> GetBasicMovesInCurBoard();
	virtual void UpdateSpecialMoves(TSet<APiece*>& EnemyPieces);
	void RemoveMoveKingCheckedByEnemies(APiece* MyKing, TSet<APiece*>& EnemyPieces);
	bool CanMoveTo(FVector Location);

	bool IsAbleToPick();
	bool IsAbleToPutAt(FVector Dest) const;
	virtual void PutAt(FVector Dest);
	bool IsSpecialMove(FVector Move) const;
	virtual void DoSpecialMove();

	bool IsFirstMove() const { return bIsFirstMove; }
	
	TSet<FVector> GetMoves() { return Moves; };

	void ShowMoves();
	void DestroyMoveBoxes();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void SetType(EPieceType PT) { PieceType = PT; }
	void AddToMoves(const FVector Location);
	void AddToSpecialMoves(const FVector Location);

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PieceMesh;

	UPROPERTY(VisibleAnywhere)
	TSet<FVector> Moves;

	UPROPERTY(VisibleAnywhere)
	TSet<FVector> SpecialMoves;

private:
	UPROPERTY(EditAnywhere, Category = PieceState)
	EPieceColor PieceColor;

	UPROPERTY(VisibleAnywhere, Category = PieceState)
	EPieceType PieceType;

	UPROPERTY(VisibleAnywhere, Category = PieceState)
	bool bIsFirstMove;

	UPROPERTY(VisibleAnywhere, Category = Moves)
	TSet<class APaperSpriteActor*> MoveBoxes;

	UPROPERTY(VisibleAnywhere, Category = Moves)
	TSubclassOf<class APaperSpriteActor> MoveBoxClass;

};
