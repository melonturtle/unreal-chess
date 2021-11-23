// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPlayerController.h"
#include "ChessPlayer.h"
#include "ChessInfo.h"
#include "ChessUtil.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"

AChessPlayerController::AChessPlayerController()
{

}

void AChessPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CurPlayer = GetWorld()->SpawnActor<AChessPlayer>(FVector(1200.0f, 2100.0f, 2200.0f), FRotator(-70.0f, -90.0f, 0.0f));
	PrevPlayer = GetWorld()->SpawnActor<AChessPlayer>(FVector(1200.0f, 300.0f, 2200.0f), FRotator(-70.0f, 90.0f, 0.0f));

	CurPlayer->SetActorLabel(FString(TEXT("WhitePlayer")));
	PrevPlayer->SetActorLabel(FString(TEXT("BlackPlayer")));

	CurPlayer->SetPlayerColor(EPieceColor::White);
	PrevPlayer->SetPlayerColor(EPieceColor::Black);

	CurPlayer->SetFolderPath("/Player");
	PrevPlayer->SetFolderPath("/Player");
	SetFolderPath("/Player");

	CurPlayer->SetPickBoxStart(FVector(1350.0f,1950.0f,PickBoxZ));
	PrevPlayer->SetPickBoxStart(FVector(1050.0f,450.0f, PickBoxZ));

	bShowMouseCursor = true;

	Swap(CurPlayer, PrevPlayer);
	OnPossess(CurPlayer);
}

void AChessPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Up"), EInputEvent::IE_Pressed, this, &AChessPlayerController::Up);
		InputComponent->BindAction(TEXT("Down"), EInputEvent::IE_Pressed, this, &AChessPlayerController::Down);
		InputComponent->BindAction(TEXT("Right"), EInputEvent::IE_Pressed, this, &AChessPlayerController::Right);
		InputComponent->BindAction(TEXT("Left"), EInputEvent::IE_Pressed, this, &AChessPlayerController::Left);
		InputComponent->BindAction(TEXT("Enter"), EInputEvent::IE_Pressed, this, &AChessPlayerController::Enter);
		InputComponent->BindAction(TEXT("Click"), EInputEvent::IE_Pressed, this, &AChessPlayerController::Click);
	}
}

void AChessPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (IsValid(CurPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("POSSESS! [%s]"), *(CurPlayer->GetActorLabel()));
		CurPlayer->SpawnPickBox();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cur Player is not valid!"));
	}
}

void AChessPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (CurPlayer->IsPicking())
	{

	}
}

void AChessPlayerController::Up()
{
	FVector CurBoxLocation = CurPlayer->GetPickBoxLocation();
	FVector Differ = UChessUtil::GetPlayerForwardVector(CurPlayer->GetPlayerColor());
	CurPlayer->MovePickBox(CurBoxLocation + Differ);
}

void AChessPlayerController::Down()
{
	FVector CurBoxLocation = CurPlayer->GetPickBoxLocation();
	FVector Differ = UChessUtil::GetPlayerForwardVector(CurPlayer->GetPlayerColor());
	CurPlayer->MovePickBox(CurBoxLocation - Differ);
}

void AChessPlayerController::Right()
{
	FVector CurBoxLocation = CurPlayer->GetPickBoxLocation();
	FVector Differ = UChessUtil::GetPlayerRightVector(CurPlayer->GetPlayerColor());
	CurPlayer->MovePickBox(CurBoxLocation + Differ);
}

void AChessPlayerController::Left()
{
	FVector CurBoxLocation = CurPlayer->GetPickBoxLocation();
	FVector Differ = UChessUtil::GetPlayerRightVector(CurPlayer->GetPlayerColor());
	CurPlayer->MovePickBox(CurBoxLocation - Differ);
}

void AChessPlayerController::Enter()
{
	if (!IsValid(CurPlayer))
		return;
	if (CurPlayer->IsPicking())
	{
		bool bIsPutSucceeded = CurPlayer->PutCurPiece();
		if (bIsPutSucceeded)
			ChangePlayer();
	}
	else
	{
		CurPlayer->PickPiece();
	}
}

void AChessPlayerController::Click()
{
	if (!IsValid(CurPlayer))
		return;
	// Trace to see what is under the mouse cursor
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, OUT HitResult);

	if (HitResult.bBlockingHit)
	{
		FVector HitPoint = HitResult.ImpactPoint;

		// For debugging
		FVector LineStart = HitPoint;
		FVector LineEnd = LineStart;
		LineEnd.Z += 100.0f;

		DrawDebugLine(
			GetWorld(), LineStart, LineEnd,
			FColor::Blue, false, 1.0f, 0.0f, 30.0f
		);

		if (UChessUtil::IsInBoard(HitPoint))
		{
			FString HitLabel = HitResult.Actor->GetActorLabel();
			// �� Board��?
			UE_LOG(LogTemp, Warning, TEXT("Mouse Hit: %s (%f, %f, %f)"), *HitLabel, HitPoint.X, HitPoint.Y, HitPoint.Z);
			FVector ClickedSquareCenter = UChessUtil::GetSquareCenter(HitPoint);
			CurPlayer->MovePickBox(ClickedSquareCenter);
			Enter();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Mouse didn't hit anything"));
	}
}

void AChessPlayerController::ChangePlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("NEXT TURN"));

	CurPlayer->DestroyPickBox();

	UnPossess();
	Swap(CurPlayer, PrevPlayer);
	OnPossess(CurPlayer);

	CurPlayer->UpdateThreatMap();

	// if Checkmate.. Check.. Stalemate... : Show UI
}
