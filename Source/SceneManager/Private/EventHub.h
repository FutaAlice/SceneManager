// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include <mutex>

#include "CoreMinimal.h"

/**
 * Singleton event hub
 */
class EventHub
{
public:
	~EventHub();
    static std::shared_ptr<EventHub> Get();

private:
    EventHub();
    EventHub(const EventHub &) = delete;
    EventHub& operator=(const EventHub &) = delete;

private:
    static std::shared_ptr<EventHub> _instance;
    static std::mutex _mutex;
};
