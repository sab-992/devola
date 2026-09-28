import { ComponentFixture, TestBed } from '@angular/core/testing';

import { AuthenticationPage } from './authentication';

describe('Authentication', () => {
    let component: AuthenticationPage;
    let fixture: ComponentFixture<AuthenticationPage>;

    beforeEach(async () => {
        await TestBed.configureTestingModule({
            imports: [AuthenticationPage],
        }).compileComponents();

        fixture = TestBed.createComponent(AuthenticationPage);
        component = fixture.componentInstance;
        await fixture.whenStable();
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });
});
