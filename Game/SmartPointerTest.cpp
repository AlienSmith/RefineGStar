#pragma once
#include"SmartPointerTest.h"
#include<list>
#include"../Utility/UniqueToManyPointer.h"
namespace GStar {
	void SmartPointerTestEnterance(){
		std::list<GStar::RootPointer<Shape>> RootList;
		std::list<GStar::SpectatorPointer<Shape>> ObserverList;
		std::list<GStar::SpectatorPointer<Shape>> SecondObserverList;
		RootList.push_back(RootPointer<Shape>::make_rootpointer());
		std::list<GStar::RootPointer<Shape>>::iterator it = RootList.begin();
		ObserverList.push_back(it->make_SpecatorPointer());
		SecondObserverList.push_back(it->make_SpecatorPointer());
		(**it).PrintType();
		std::list<GStar::SpectatorPointer<Shape>>::iterator obit = ObserverList.begin();
		(**obit).PrintType();
		std::list<GStar::SpectatorPointer<Shape>>::iterator obsit = SecondObserverList.begin();
		(**obsit).PrintType();
		RootList.pop_back();
		ObserverList.pop_back();
		SecondObserverList.pop_back();
		for (int i = 0; i < 10; i++) {
			if (i % 2 == 0) {
				RootList.push_back(RootPointer<Shape>::make_rootpointer());
			}
			else {
				RootList.push_back(RootPointer<Squre>::make_rootpointer());
			}
		}
		for (std::list<GStar::RootPointer<Shape>>::iterator it = RootList.begin(); it != RootList.end(); it++) {
			ObserverList.push_back(it->make_SpecatorPointer());
			SecondObserverList.push_back(it->make_SpecatorPointer());
		}
		int i = 0;
		//Remove every third item
		for (std::list<GStar::RootPointer<Shape>>::iterator it = RootList.begin(); it != RootList.end(); ) {
			it = RootList.erase(it);
			if (it == RootList.end()) {
				break;
			}
			++it;
			if (it == RootList.end()) {
				break;
			}
			++it;
		}
		for (std::list<GStar::SpectatorPointer<Shape>>::iterator it = ObserverList.begin(); it != ObserverList.end();) {
			if ((*it).isValid()) {
				(**it).PrintType();
				++it;
			}
			else {
				it = ObserverList.erase(it);
			}
		}
		for (std::list<GStar::SpectatorPointer<Shape>>::iterator it = SecondObserverList.begin(); it != SecondObserverList.end();) {
			if ((*it).isValid()) {
				(**it).PrintType();
				++it;
			}
			else {
				it = SecondObserverList.erase(it);
			}
		}
		printf("%zu Rootlist\n",RootList.size());
		printf("%zu ObserverList\n", ObserverList.size());
		printf("%zu SecondObserverList\n", SecondObserverList.size());
	}
}