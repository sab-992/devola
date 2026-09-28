import { ComponentFixture, TestBed } from '@angular/core/testing';

import { FeedPage } from './feed';

describe('Feed', () => {
    let component: FeedPage;
    let fixture: ComponentFixture<FeedPage>;

    beforeEach(async () => {
        await TestBed.configureTestingModule({
            imports: [FeedPage],
        }).compileComponents();

        fixture = TestBed.createComponent(FeedPage);
        component = fixture.componentInstance;
        await fixture.whenStable();
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });
});
