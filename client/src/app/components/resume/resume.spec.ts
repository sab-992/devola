import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ResumePage } from './resume';

describe('ResumePage', () => {
    let component: ResumePage;
    let fixture: ComponentFixture<ResumePage>;

    beforeEach(async () => {
        await TestBed.configureTestingModule({
            imports: [ResumePage],
        }).compileComponents();

        fixture = TestBed.createComponent(ResumePage);
        component = fixture.componentInstance;
        await fixture.whenStable();
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });
});
